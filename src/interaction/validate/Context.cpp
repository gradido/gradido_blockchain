#include "gradido_blockchain/const.h"
#include "gradido_blockchain/data/compact/ConfirmedGradidoTx.h"
#include "gradido_blockchain/data/compact/ConfirmedGradidoTxCold.h"
#include "gradido_blockchain/data/BalanceDerivationType.h"
#include "gradido_blockchain/data/CrossGroupType.h"
#include "gradido_blockchain/data/TransactionType.h"
#include "gradido_blockchain/interaction/validate/ContextData.h"
#include "gradido_blockchain/blockchain/AbstractProvider.h"
#include "gradido_blockchain/blockchain/Exceptions.h"
#include "gradido_blockchain/interaction/validate/ConfirmedTransactionRole.h"
#include "gradido_blockchain/interaction/validate/Context.h"
#include "gradido_blockchain/interaction/validate/GradidoTransactionRole.h"
#include "gradido_blockchain/interaction/validate/TransactionBodyRole.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

#include "magic_enum/magic_enum.hpp"

#include <string>

using DataTypeConverter::timespanToString, DataTypeConverter::timePointToString;
using magic_enum::enum_name;
using std::string, std::to_string;

namespace gradido {
	using data::compact::ConfirmedGradidoTx;
	using data::BalanceDerivationType;
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

		static Error validatePreviousAll(const ConfirmedGradidoTx& tx, const AppContext& appContext, Options options)
		{

		}

		static Error validateSingleCommunityRoot(const ConfirmedGradidoTx& tx, const AppContext& appContext, Options options)
		{
			if (!tx.isCommunityRoot()) {
				throw GradidoNodeInvalidDataException("called validateSingleCommunityRoot with not community root typed tx");
			}
			const auto& communityRoot = tx.specific.communityRoot;
			const auto& publicKeyIdDict = appContext.getCommunityContext(tx.txCommunityIdIndex).getBlockchain()->getPublicKeyDictionary();

			if (!publicKeyIdDict.hasIndex(communityRoot.publicKeyIndex)
				|| !publicKeyIdDict.hasIndex(communityRoot.gmwPublicKeyIndex)
			  || !publicKeyIdDict.hasIndex(communityRoot.aufPublicKeyIndex)) {
				return { .type = ErrorType::Invalid_Dictionary_Index, .message = "at least one of publicKeyIndex, gmwPublicKeyIndex or aufPublicKeyIndex couldn't be found in PublicKey Dictionary" };
			}
			if (communityRoot.publicKeyIndex == communityRoot.aufPublicKeyIndex ||
				communityRoot.publicKeyIndex == communityRoot.gmwPublicKeyIndex ||
				communityRoot.gmwPublicKeyIndex == communityRoot.aufPublicKeyIndex) {
				return { .type = ErrorType::Field_Value_Conflict, .message = "at least two of publicKeyIndex, gmwPublicKeyIndex or aufPublicKeyIndex are identical " };
			}

			if (tx.hasColdData()) {
				const auto& coldData = tx.coldData.get();
				if (coldData->signatureMap.size() != 1) {
					return {
						.type = ErrorType::Invalid_Field,
						.message = "unexpected signature count",
						.actual = to_string(coldData->signatureMap.size()),
						.expected = "1"
					};
				}
				auto publicKey = publicKeyIdDict.getDataForIndexOrThrow(communityRoot.publicKeyIndex);
				if (!coldData->signatureMap[0].first.isTheSame(publicKey)) {
					return {
						.type = ErrorType::Invalid_Field,
						.message = "wrong signer",
						.actual = coldData->signatureMap[0].first.convertToHex(),
						.expected = publicKey.convertToHex()
					};
				}
			}

			if (tx.isConfirmedTx()) {
				if (tx.txNr != 1) {
					return { .type = ErrorType::Invalid_Field, .message = "CommunityRoot must be first tx with tx nr = 1" };
				}
				if (tx.accountBalanceCount != 2) {
					return { .type = ErrorType::Invalid_Field, .message = "unexpected account balances, expect gmw and auf on community root transaction" };
				}
				for (int i = 0; i < tx.accountBalanceCount; i++) {
					const auto& accountBalance = tx.accountBalances[i];
					if (accountBalance.balanceGddCent) {
						return { 
							.type = ErrorType::Invalid_Field, 
							.message = "CommunityRoot starts with empty account balances", 
							.actual = to_string(accountBalance.balanceGddCent),
							.expected = "0"
						};
					}
					if (accountBalance.coinCommunityIdIndex != tx.txCommunityIdIndex) {
						return {
							.type = ErrorType::Invalid_Field,
							.message = "CommunityRoot account balances aren't allowed to have diff coin community id",
							.actual = to_string(accountBalance.coinCommunityIdIndex),
							.expected = to_string(tx.txCommunityIdIndex)
						};
					}
					if (accountBalance.publicKeyIndex != communityRoot.gmwPublicKeyIndex && accountBalance.publicKeyIndex != communityRoot.aufPublicKeyIndex) {
						string expected = to_string(communityRoot.gmwPublicKeyIndex);
						expected += ", or ";
						expected += to_string(communityRoot.aufPublicKeyIndex);
						return {
							.type = ErrorType::Invalid_Field,
							.message = "CommunityRoot account balances need to belong either to gmw or to auf public key",
							.actual = to_string(accountBalance.publicKeyIndex),
							.expected = expected
						};
					}
				}
			}
			return { .type = ErrorType::Success };
		}

		static Error validateSingleCreation(const ConfirmedGradidoTx& tx, const AppContext& appContext, Options options)
		{
			if (tx.isCreation()) {
				throw GradidoNodeInvalidDataException("called validateSingleCreation with not creation typed tx");
			}
			const auto& creation = tx.specific.creation;
			const auto& publicKeyIdDict = appContext.getCommunityContext(tx.txCommunityIdIndex).getBlockchain()->getPublicKeyDictionary();
			if (!publicKeyIdDict.hasIndex(creation.recipientPublicKeyIndex)) {
				return { .type = ErrorType::Invalid_Dictionary_Index, .message = "couldn't find recipient public key in Dictionary" };
			}
			if (creation.amountGddCent < 2'000) {
				return {
					.type = ErrorType::Invalid_Field,
					.message = "creation amount to low, min 0.2 GDD (2000 GDD Cent)",
					.actual = to_string(creation.amountGddCent),
					.expected = ">= 2000"
				};
			}
			if (creation.amountGddCent > 10'000'000) {
				return {
					.type = ErrorType::Invalid_Field,
					.message = "creation amount to high, max 1000 per month (10'000'000 GDD Cent)",
					.actual = to_string(creation.amountGddCent),
					.expected = "<= 10'000'000"
				};
			}
			if (tx.hasColdData()) {
				const auto& coldData = tx.coldData.get();
				if (coldData->signatureMap.size() != 1) {
					return {
						.type = ErrorType::Invalid_Field,
						.message = "unexpected signature count",
						.actual = to_string(coldData->signatureMap.size()),
						.expected = "1"
					};
				}
				auto signerPublicKeyIndex = publicKeyIdDict.getIndexForData(coldData->signatureMap[0].first);
				if (signerPublicKeyIndex == creation.recipientPublicKeyIndex) {
					return {
						.type = ErrorType::Invalid_Field,
						.message = "creation must be signed from other public key as the recipient"
					};
				}
			}

			if (tx.isConfirmedTx()) {
				if (tx.txNr < 3) {
					return {
						.type = ErrorType::Invalid_Field,
						.message = "Creation must be tx nr 3 at least to have community root, recipient and someone to sign registered first",
						.actual = to_string(tx.txNr),
						.expected = "< 3"
					};
				}
				if (tx.accountBalanceCount != 3) {
					return { .type = ErrorType::Invalid_Field, .message = "unexpected account balances, expect recipient, gmw and auf on creation" };
				}
				bool creationAccountBalanceFound = false;
				for (int i = 0; i < tx.accountBalanceCount; i++)
				{
					const auto& accountBalance = tx.accountBalances[i];
					if (accountBalance.balanceGddCent < creation.amountGddCent) {
						return {
							.type = ErrorType::Invalid_Field,
							.message = "Creation account balance couldn't be smaller than creation amount",
							.actual = to_string(accountBalance.balanceGddCent),
							.expected = ">= " + to_string(creation.amountGddCent)
						};
					}
					if (accountBalance.coinCommunityIdIndex != tx.txCommunityIdIndex) {
						return {
							.type = ErrorType::Invalid_Field,
							.message = "Creation account balances aren't allowed to have diff coin community id",
							.actual = to_string(accountBalance.coinCommunityIdIndex),
							.expected = to_string(tx.txCommunityIdIndex)
						};
					}
					if (accountBalance.publicKeyIndex == creation.recipientPublicKeyIndex) {
						if (creationAccountBalanceFound) {
							return { .type = ErrorType::Field_Value_Conflict, .message = "more than one account balance for creation recipient" };
						}
						creationAccountBalanceFound = true;
					}
				}
				if (!creationAccountBalanceFound) {
					return { .type = ErrorType::Field_Value_Conflict, .message = "missing account balance for creation recipient" };
				}
			}
			return { .type = ErrorType::Success };
		}

		static Error validateSingleTransfer(const ConfirmedGradidoTx& tx, const AppContext& appContext, Options options)
		{

		}

		// all checks which don't need other transactions
		static Error validateSingleCommon(const ConfirmedGradidoTx& tx, const AppContext& appContext, Options options) 
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
				if (coldData) {
					if (std::chrono::abs(coldData->getCreatedAt().getAsTimepoint() - tx.getConfirmedAt().getAsTimepoint())
							> MAGIC_NUMBER_MAX_TIMESPAN_BETWEEN_CREATING_AND_RECEIVING_TRANSACTION) {
						string message = 
							"timespan between created at and confirmed at are more than "
							+ timespanToString(MAGIC_NUMBER_MAX_TIMESPAN_BETWEEN_CREATING_AND_RECEIVING_TRANSACTION)
						;
						string actual =
							"createdAt: "
							+ timePointToString(coldData->getCreatedAt())
							+ ", confirmedAt: "
							+ timePointToString(tx.getConfirmedAt())
						;
						string expected = 
							timePointToString(tx.getConfirmedAt())
							+ " <= (" + timePointToString(coldData->getCreatedAt())
							+ " +- " + timespanToString(MAGIC_NUMBER_MAX_TIMESPAN_BETWEEN_CREATING_AND_RECEIVING_TRANSACTION)
							+ ")"
						;
						return { .type = ErrorType::Invalid_Field, .message = message, .actual = actual, .expected = expected };
					}
					if (coldData->ledgerAnchor.empty()) {
						return  {
							.type = ErrorType::Invalid_Field,
							.message = "empty ledger anchor"
						};
					}
				}
			}
			// from here an for confirmed and not confirmed transactions
			
			// check if it is a cross community transaction, that needed informations are there
			if (CrossGroupType::LOCAL != tx.crossGroupType) 
			{
				if (TransactionType::COMMUNITY_FRIENDS_UPDATE != tx.transactionType &&
					TransactionType::REDEEM_DEFERRED_TRANSFER != tx.transactionType &&
					TransactionType::TRANSFER != tx.transactionType) 
				{
					return { 
						.type = ErrorType::Invalid_Transaction_Type, 
						.message = "cross group transactions (currently) only possible with Transfer, Redeem Deferred Transfer and Community Friends Update" 
					};
				}
				if (coldData) {
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
			if (BalanceDerivationType::UNSPECIFIED == tx.balanceDerivationType) {
				return { .type = ErrorType::Invalid_Field, .message = "balanceDerivationType is unspecified" };
			}
			if (coldData) {
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
							return { .type = ErrorType::Crypto_Sign_Invalid, .message = "on of the signatures cannot be verified" };
						}
					}
				}
				if (coldData->bodyBytes.isEmpty()) {
					return { .type = ErrorType::Invalid_Field, .message = "empty body bytes" };
				}
			}
			return { .type = ErrorType::Success };
		}

		Error validate(const ConfirmedGradidoTx& tx, const AppContext& appContext, Options options) 
		{
			tx.isConfirmedTx();
			Error result;
			if ((Type::SINGLE & options.type) == Type::SINGLE) {
				result = validateSingleCommon(tx, appContext, options);
				if (ErrorType::Success != result.type) {
					return result;
				}
				switch (tx.transactionType) {
				case TransactionType::CREATION:
					result = validateSingleCreation(tx, appContext, options);
					break;
				case TransactionType::COMMUNITY_ROOT:
					result = validateSingleCommunityRoot(tx, appContext, options);
					break;
				default: 
					throw GradidoUnhandledEnum(
						"single validation for transaction type missing",
						"TransactionType",
						enum_name(tx.transactionType).data()
					);
				}
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