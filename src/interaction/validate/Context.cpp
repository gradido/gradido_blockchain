#include "gradido_blockchain/data/compact/ConfirmedGradidoTx.h"
#include "gradido_blockchain/data/compact/ConfirmedGradidoTxCold.h"
#include "gradido_blockchain/data/CrossGroupType.h"
#include "gradido_blockchain/data/TransactionType.h"
#include "gradido_blockchain/interaction/validate/ContextData.h"
#include "gradido_blockchain/blockchain/AbstractProvider.h"
#include "gradido_blockchain/blockchain/Exceptions.h"
#include "gradido_blockchain/interaction/validate/ConfirmedTransactionRole.h"
#include "gradido_blockchain/interaction/validate/Context.h"
#include "gradido_blockchain/interaction/validate/GradidoTransactionRole.h"
#include "gradido_blockchain/interaction/validate/TransactionBodyRole.h"

#include <string>

using std::to_string;

namespace gradido {
	using data::compact::ConfirmedGradidoTx;
	using data::CrossGroupType;
	using data::TransactionType;

	namespace interaction::validate {

		Context::Context(const data::TransactionBody& body)
			: mRole(std::make_unique<TransactionBodyRole>(body)), mDisableVerify(false), mDisableRunningHashTest(false)
		{
		}

		Context::Context(const data::GradidoTransaction& body)
			: mRole(std::make_unique<GradidoTransactionRole>(body)), mDisableVerify(false), mDisableRunningHashTest(false)
		{
		}

		Context::Context(const data::ConfirmedTransaction& body)
			: mRole(std::make_unique<ConfirmedTransactionRole>(body)), mDisableVerify(false), mDisableRunningHashTest(false)
		{
		}

		Context::~Context()
		{

		}

		void Context::run(Type type/* = Type::SINGLE*/, std::shared_ptr<blockchain::Abstract> blockchain /*= nullptr*/)
		{
			if (!mSenderPreviousConfirmedTransaction && blockchain) {
				auto transactionEntry = blockchain->findOne(blockchain::Filter::LAST_TRANSACTION);
				if (transactionEntry) {
					mSenderPreviousConfirmedTransaction = transactionEntry->getConfirmedTransaction();
				}
			}
			ContextData c(blockchain, mSenderPreviousConfirmedTransaction);
			if (mRecipientPreviousConfirmedTransaction) {
				c.recipientPreviousConfirmedTransaction = mRecipientPreviousConfirmedTransaction;
			}
			if (mDisableVerify) {
				mRole->disableVerify();
			}
			if (mDisableRunningHashTest) {
				mRole->disableRunningHashTest();
			}
			mRole->run(type, c);
		}

		// all checks which don't need other transactions
		static Error validateSingle(const ConfirmedGradidoTx& tx, const AppContext& appContext, Options options) 
		{
			const auto coldData = tx.coldData.get();
			const auto& communityIdDict = appContext.getCommunityIds();

			if (options.errorOnMissingColdData && !tx.hasColdData()) {
				return { .type = ErrorType::Missing_Cold_Data, .message = "missing cold data with option errorOnMissingColdData = true" };
			}
			// check only for confirmed transactions
			if (tx.isConfirmedTx()) 
			{
				if (!tx.txNr) {
					return { .type = ErrorType::Invalid_Field, .message = "txNr is invalid", .actual = "0", .expected = "> 0"};
				}
				// 946681200 = 01.01.2000
				if (tx.confirmedAtSeconds < 946681200) {
					return {
						.type = ErrorType::Invalid_Field,
						.message = "confirmedAt.seconds is to small",
						.actual = to_string(tx.confirmedAtSeconds),
						.expected = "> 946681200 (01.01.2000)"
					};
				}
				
				// check if community id index is valid, therefore an entry exist in Community Id Dictionary
				if (!tx.txCommunityIdIndex) {
					return { .type = ErrorType::Invalid_Field, .message = "invald tx community id index", .actual = "0", .expected = "> 0" };
				}
				if (!communityIdDict.hasIndex(tx.txCommunityIdIndex)) {
					return { .type = ErrorType::Invalid_Dictionary_Index, .message = "txCommunityIdIndex not in dictionary" };
				}
				// the same for coin community id, but only if value is != 0, because 0 means here, not different coin community id than txCommunityId
				if (tx.coinCommunityIdIndex && !communityIdDict.hasIndex(tx.coinCommunityIdIndex)) {
					return { .type = ErrorType::Invalid_Dictionary_Index, .message = "coinCommunityIdIndex not in dictionary" };
				}
				if (tx.hasColdData()) {
					for (auto& sigPair : coldData->signatureMap) {
						if (sigPair.first.isEmpty()) {
							return { .type = ErrorType::Invalid_Field, .message = "empty public key in signature map" };
						}
						if (sigPair.second.isEmpty()) {
							return { .type = ErrorType::Invalid_Field, .message = "empty signature in signature map" };
						}
						if (options.enableVerify) {
							// TODO: move into PublicKey or ed25519KeyPair
							if (crypto_sign_verify_detached(
								(const unsigned char*)sigPair.second.data(),
								(const unsigned char*)coldData->bodyBytes.data(), 
								coldData->bodyBytes.size(), 
								sigPair.first.data()
							) != 0) {
								return { .type = ErrorType::Crypto_Sign_Invalid, .message = "on of the signatures cannot be verfied" };
							}
						}
					}
					if (coldData->ledgerAnchor.empty()) {
						return  {
							.type = ErrorType::Invalid_Field,
							.message = "empty ledger anchor"
						};
					}
				}
			}
			// check if it is a cross community transaction, that needed informations are there
			if (CrossGroupType::LOCAL != tx.crossGroupType) {
				if (TransactionType::COMMUNITY_FRIENDS_UPDATE != tx.transactionType &&
					TransactionType::REDEEM_DEFERRED_TRANSFER != tx.transactionType &&
					TransactionType::TRANSFER != tx.transactionType) {
					return { 
						.type = ErrorType::Invalid_Transaction_Type, 
						.message = "cross group transactions (currently) only possible with Transfer, Redeem Deferred Transfer and Community Friends Update" 
					};
				}
				if (tx.hasColdData() && coldData) {
					if (!coldData->pairingTxCommunityIdIndex) {
						return {
							.type = ErrorType::Invalid_Field,
							.message = "missing pairing community id index for cross group transaction"
						};
					}
					if (!communityIdDict.hasIndex(coldData->pairingTxCommunityIdIndex)) {
						return {
							.type = ErrorType::Invalid_Dictionary_Index,
							.message = "couldn't found pairing community id index for cross group transaction in dictionary"
						};
					}
					if (CrossGroupType::INBOUND == tx.crossGroupType && coldData->pairingLedgerAnchor.empty()) {
						return {
							.type = ErrorType::Invalid_Field,
							.message = "empty pairing ledger anchor for Inbound Cross Group Transaction"
						};
					}
				}
			}
			if (tx.hasColdData()) {
				for (auto& sigPair : coldData->signatureMap) {
					if (sigPair.first.isEmpty()) {
						return { .type = ErrorType::Invalid_Field, .message = "empty public key in signature map" };
					}
					if (sigPair.second.isEmpty()) {
						return { .type = ErrorType::Invalid_Field, .message = "empty signature in signature map" };
					}
					if (options.enableVerify) {
						// TODO: move into PublicKey or ed25519KeyPair
						if (crypto_sign_verify_detached(
							(const unsigned char*)sigPair.second.data(),
							(const unsigned char*)coldData->bodyBytes.data(),
							coldData->bodyBytes.size(),
							sigPair.first.data()
						) != 0) {
							return { .type = ErrorType::Crypto_Sign_Invalid, .message = "on of the signatures cannot be verfied" };
						}
					}
				}
			}
		}

		Error validate(const ConfirmedGradidoTx& tx, const AppContext& appContext, Options options) 
		{
			tx.isConfirmedTx();
			Error result;
			if ((Type::SINGLE & options.type) == Type::SINGLE) {
				result = validateSingle(tx, appContext, options);
				if (ErrorType::Success != result.type) {
					return result;
				}
			}
			return {
				.type = ErrorType::Success
			};
		}
	}
}