#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/CommunityContext.h"
#include "gradido_blockchain/data/compact/CommunityRootTx.h"
#include "gradido_blockchain/data/compact/ConfirmedGradidoTx.h"
#include "gradido_blockchain/data/compact/ConfirmedGradidoTxCold.h"
#include "gradido_blockchain/interaction/validate/CommunityRootRole.h"
#include "gradido_blockchain/interaction/validate/Error.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/interaction/validate/Options.h"
#include "gradido_blockchain/memory/Block.h"

#include "date/date.h"

#include <memory>
#include <string>

using memory::ConstBlockPtr;
using std::shared_ptr, std::make_shared;
using std::string, std::to_string;


namespace gradido {
	using data::ConfirmedTransaction;
	using data::compact::CommunityRootTx, data::compact::ConfirmedGradidoTx, data::compact::ConfirmedGradidoTxCold;
	namespace interaction::validate {

		CommunityRootRole::CommunityRootRole(CommunityRootTx&& communityRoot, uint32_t communityIdIndex)
			: mCommunityRoot(std::move(communityRoot)), mCommunityIdIndex(communityIdIndex)
		{
			// prepare for signature check
			mMinSignatureCount = 1;
			mRequiredSignPublicKeyIndices[0] = { .communityIdIndex = mCommunityIdIndex, .publicKeyIndex = communityRoot.publicKeyIndex };
			mRequiredSignPublicKeyIndicesCount = 1;
		}

		void CommunityRootRole::run(Type type, ContextData& c) 
		{
			if ((type & Type::SINGLE) == Type::SINGLE) {
				const auto& dict = g_appContext->getCommunityContext(mCommunityIdIndex).getBlockchain()->getPublicKeyDictionary();
				if (
					!dict.hasIndex(mCommunityRoot.publicKeyIndex) ||
					!dict.hasIndex(mCommunityRoot.gmwPublicKeyIndex) ||
					!dict.hasIndex(mCommunityRoot.aufPublicKeyIndex)
					) {
					throw TransactionValidationException("at least one public key index is invalid");
				}
					
				if (mCommunityRoot.gmwPublicKeyIndex == mCommunityRoot.aufPublicKeyIndex) {
					throw TransactionValidationException("gmw and auf are the same"); 
				}
				if (mCommunityRoot.publicKeyIndex == mCommunityRoot.gmwPublicKeyIndex) { 
					throw TransactionValidationException("gmw and pubkey are the same"); 
				}
				if (mCommunityRoot.aufPublicKeyIndex == mCommunityRoot.publicKeyIndex) { 
					throw TransactionValidationException("aufPubkey and pubkey are the same");
				}
			}
			if ((type & Type::PREVIOUS) == Type::PREVIOUS) {
				if (c.senderPreviousConfirmedTransaction) {
					throw TransactionValidationException("community root must be the first transaction in the blockchain!");
				}
			}
		}

		static Error validateSignatures(
			const CommunityRootTx& communityRoot, 
			const ConfirmedGradidoTxCold* coldData, 
			const IDictionary<PublicKey>& publicKeyDictionary
		) {
			if (coldData->signatureMap.size() != 1) {
				return {
					.type = ErrorType::Invalid_Field,
					.message = "unexpected signature count",
					.actual = to_string(coldData->signatureMap.size()),
					.expected = "1"
				};
			}
			auto publicKey = publicKeyDictionary.getDataForIndexOrThrow(communityRoot.publicKeyIndex);
			if (!coldData->signatureMap[0].first.isTheSame(publicKey)) {
				return {
					.type = ErrorType::Invalid_Field,
					.message = "wrong signer",
					.actual = coldData->signatureMap[0].first.convertToHex(),
					.expected = publicKey.convertToHex()
				};
			}
			return { .type = ErrorType::Success };
		}

		static Error validateAccountBalancesSingle(const ConfirmedGradidoTx& tx)
		{
			const auto& communityRoot = tx.specific.communityRoot;
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
			return { .type = ErrorType::Success };
		}
	
		static Error validateSingle(const ConfirmedGradidoTx& tx, const shared_ptr<const blockchain::Abstract>& blockchain, Options options)
		{
			const auto& communityRoot = tx.specific.communityRoot;
			const auto& publicKeyIdDict = blockchain->getPublicKeyDictionary();

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
			Error result = { .type = ErrorType::Success };
			if (tx.hasColdData()) {
				result = validateSignatures(communityRoot, tx.coldData.get(), publicKeyIdDict);
			}
			if (ErrorType::Success == result.type && tx.isConfirmedTx()) {
				if (tx.txNr != 1) {
					return { .type = ErrorType::Invalid_Field, .message = "CommunityRoot must be first tx with tx nr = 1" };
				}
				result = validateAccountBalancesSingle(tx);
			}
			return result;
		}

		Error validateCommunityRoot(const ConfirmedGradidoTx& tx, const shared_ptr<const blockchain::Abstract>& blockchain, Options options)
		{
			if (!tx.isCommunityRoot()) {
				throw GradidoNodeInvalidDataException("called validateSingleCommunityRoot with not community root typed tx");
			}
			Error result;
			if ((Type::SINGLE & options.type) == Type::SINGLE) {
				result = validateSingle(tx, blockchain, options);
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