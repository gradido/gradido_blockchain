#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/CompactFilter.h"
#include "gradido_blockchain/blockchain/FilterBuilder.h"
#include "gradido_blockchain/blockchain/SearchDirection.h"
#include "gradido_blockchain/blockchain/TransactionEntry.h"
#include "gradido_blockchain/data/adapter/publicKey.h"
#include "gradido_blockchain/data/AddressType.h"
#include "gradido_blockchain/data/compact/ConfirmedGradidoTx.h"
#include "gradido_blockchain/data/compact/ConfirmedGradidoTxCold.h"
#include "gradido_blockchain/data/compact/PublicKeyIndex.h"
#include "gradido_blockchain/data/compact/RegisterAddressTx.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/data/RegisterAddress.h"
#include "gradido_blockchain/data/SignatureMap.h"
#include "gradido_blockchain/data/TransactionType.h"
#include "gradido_blockchain/interaction/validate/Error.h"
#include "gradido_blockchain/interaction/validate/Options.h"
#include "gradido_blockchain/interaction/validate/RegisterAddressRole.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/memory/Block.h"

#include "date/date.h"
#include "magic_enum/magic_enum.hpp"

#include <memory>
#include <optional>
#include <string>
#include <vector>

using memory::Block, memory::ConstBlockPtr;
using std::optional, std::nullopt;
using std::shared_ptr, std::make_shared;
using std::string, std::to_string;
using std::vector;

using namespace magic_enum;

namespace gradido {
	using blockchain::CompactFilter, blockchain::Filter, blockchain::FilterBuilder, blockchain::SearchDirection, blockchain::TransactionEntry;
	using data::adapter::toConstBlockPtr;
	using data::compact::ConfirmedGradidoTx, data::compact::PublicKeyIndex;
	using data::AddressType, data::RegisterAddress, data::SignatureMap, data::TransactionType;
	namespace interaction::validate {

		RegisterAddressRole::RegisterAddressRole(const data::compact::RegisterAddressTx& registerAddress, uint32_t communityIdIndex)
			: mRegisterAddress(registerAddress), mCommunityIdIndex(communityIdIndex)
		{
			mRequiredSignPublicKeyIndices[0] = { 
				.communityIdIndex = mCommunityIdIndex, 
				.publicKeyIndex = registerAddress.accountPublicKeyIndex 
			};
			mRequiredSignPublicKeyIndices[1] = {
				.communityIdIndex = mCommunityIdIndex,
				.publicKeyIndex = registerAddress.userPublicKeyIndex
			};
			mMinSignatureCount = 3;
			mRequiredSignPublicKeyIndicesCount = 2;
		}

		void RegisterAddressRole::run(Type type, ContextData& c)
		{
			auto addressType = mRegisterAddress.addressType;
			auto accountPubkeyIndex = mRegisterAddress.accountPublicKeyIndex;
			auto userPubkeyIndex = mRegisterAddress.userPublicKeyIndex;

			if (AddressType::COMMUNITY_PROJECT == addressType ||
				AddressType::COMMUNITY_HUMAN == addressType) {
			}
			if ((type & Type::SINGLE) == Type::SINGLE)
			{
				if (AddressType::COMMUNITY_GMW == addressType ||
					AddressType::COMMUNITY_AUF == addressType ||
					AddressType::NONE == addressType) 
				{
					optional<uint32_t> communityIdIndex = std::nullopt;
					if (c.senderBlockchain) {
						communityIdIndex = c.senderBlockchain->getCommunityIdIndex();
					}
					throw WrongAddressTypeException(
						"register address transaction not allowed with community auf or gmw account or None",
						addressType,
						userPubkeyIndex,
						communityIdIndex
					);
				}
				const auto& dict = g_appContext->getCommunityContext(mCommunityIdIndex).getBlockchain()->getPublicKeyDictionary();
				if (!dict.hasIndex(accountPubkeyIndex)) {
					throw TransactionValidationInvalidInputException("missing key for index", "account public key");
				}
				if (!dict.hasIndex(userPubkeyIndex)) {
					throw TransactionValidationInvalidInputException("missing key for index", "user public key");
				}
				if (accountPubkeyIndex == userPubkeyIndex) {
					throw TransactionValidationException("accountPubkey and userPubkey are the same");
				}
			}

			if ((type & Type::ACCOUNT) == Type::ACCOUNT)
			{
				assert(c.senderBlockchain);
				FilterBuilder filterBuilder;

				shared_ptr<const TransactionEntry> transactionWithSameAddress;
				if (!c.senderPreviousConfirmedTransaction) {
					throw GradidoNullPointerException(
						"missing previous confirmed transaction for sender in interaction::validate RegisterAddress Type::ACCOUNT",
						"data::ConstConfirmedTransactionPtr",
						__FUNCTION__
					);
				}
				if (AddressType::SUBACCOUNT == addressType) {
					transactionWithSameAddress = c.senderBlockchain->findOne(
						filterBuilder
						.setInvolvedPublicKey(toConstBlockPtr({ .communityIdIndex = mCommunityIdIndex, .publicKeyIndex = userPubkeyIndex }))
						.setMaxTransactionNr(c.senderPreviousConfirmedTransaction->getId())
						.setSearchDirection(SearchDirection::DESC)
						.build()
					);
					if (!transactionWithSameAddress) {
						throw AddressAlreadyExistException(
							"cannot register sub address because user is missing",
							to_string(userPubkeyIndex),
							addressType
						);
					}
					transactionWithSameAddress.reset();
				}
				else {
					Filter f;
					f.involvedPublicKey = toConstBlockPtr({ .communityIdIndex = mCommunityIdIndex, .publicKeyIndex = userPubkeyIndex });
					f.maxTransactionNr = c.senderPreviousConfirmedTransaction->getId();
					f.transactionType = TransactionType::REGISTER_ADDRESS;
					auto addressType = c.senderBlockchain->getAddressType(f);
					if (AddressType::NONE != addressType) {
						throw AddressAlreadyExistException(
							"cannot register address because it already exist",
							to_string(userPubkeyIndex),
							addressType
						);
					}
				}
			}
		}

		void RegisterAddressRole::checkRequiredSignatures(
			const SignatureMap& signatureMap,
			shared_ptr<blockchain::Abstract> blockchain /*  = nullptr*/
		) const
		{
			AbstractRole::checkRequiredSignatures(signatureMap, blockchain);
			if (!blockchain) return;
			auto& signPairs = signatureMap.getSignaturePairs();

			// get community root transaction
			auto communityRootTx = blockchain->findOne(Filter::FIRST_TRANSACTION);
			if (!communityRootTx) {
				throw BlockchainOrderException("cannot find community root transaction before register address");
			}
			auto communityRoot = communityRootTx->getTransactionBody()->getCommunityRoot();
			if (!communityRoot) {
				throw GradidoNodeInvalidDataException("first transaction isn't valid community root transaction");
			}
			bool foundCommunityRootSigner = false;

			// check for account type
			for (auto& signPair : signPairs) {
				auto signPublicKey = signPair.getPublicKey();
				if (signPublicKey->isTheSame(PublicKeyIndex{ .communityIdIndex = mCommunityIdIndex, .publicKeyIndex = mRegisterAddress.accountPublicKeyIndex }) ||
					signPublicKey->isTheSame(PublicKeyIndex{ .communityIdIndex = mCommunityIdIndex, .publicKeyIndex = mRegisterAddress.userPublicKeyIndex })) {
					continue;
				}
				if (signPublicKey->isTheSame(PublicKeyIndex{ .communityIdIndex = mCommunityIdIndex, .publicKeyIndex = communityRoot->publicKeyIndex })) {
					foundCommunityRootSigner = true;
					break;
				}
			}
			if (!foundCommunityRootSigner) {
				throw TransactionValidationRequiredSignMissingException({ data::compact::PublicKeyIndex{ communityRoot->publicKeyIndex } });
			}
		}
		
		static Error validateSingleRegisterAddress(const ConfirmedGradidoTx& tx, const AppContext& appContext, Options options)
		{			
			const auto& registerAddress = tx.specific.registerAddress;
			const auto& publicKeyIdDict = appContext.getCommunityContext(tx.txCommunityIdIndex).getBlockchain()->getPublicKeyDictionary();
			if (publicKeyIdDict.hasIndex(registerAddress.accountPublicKeyIndex)) {
				return { .type = ErrorType::Invalid_Dictionary_Index, .message = "couldn't find account public key in Dictionary" };
			}
			if (publicKeyIdDict.hasIndex(registerAddress.userPublicKeyIndex)) {
				return { .type = ErrorType::Invalid_Dictionary_Index, .message = "couldn't find user public key in Dictionary" };
			}
			if (!appContext.getUserNameHashs().hasIndex(registerAddress.nameHashIndex)) {
				return { .type = ErrorType::Invalid_Dictionary_Index, .message = "couldn't user name hash in Dictionary" };
			}
			if (AddressType::COMMUNITY_HUMAN != registerAddress.addressType) {
				return {
					.type = ErrorType::Not_Implemented_Yet,
					.message = "Address Type currently not supported for register address tx",
					.actual = string(enum_name(registerAddress.addressType)),
					.expected = "COMMUNITY_HUMAN"
				};
			}
			if (tx.isCrossCommunityTx()) {
				return { .type = ErrorType::Field_Value_Conflict, .message = "Currently no Cross Community Register Adress transactions supported!" };
			}
			if (registerAddress.derivationIndex != 1) {
				return { .type = ErrorType::Not_Implemented_Yet, .message = "multiple accounts per User currently not implemented yet" };
			}
			if (tx.isConfirmedTx()) {
				if (tx.accountBalanceCount != 1) {
					return {
						.type = ErrorType::Field_Value_Conflict,
						.message = "expect only account balance of account on register address",
						.actual = to_string(tx.accountBalanceCount),
						.expected = "1"
					};
				}
				if (tx.accountBalances[0].balanceGddCent != 0) {
					return {
						.type = ErrorType::Not_Implemented_Yet,
						.message = "expect account balance of register address to start with 0 as long moving isn't implented yet",
						.actual = to_string(tx.accountBalances[0].balanceGddCent),
						.expected = "0"
					};
				}
			}
			if (tx.hasColdData()) {
				const auto& coldData = tx.coldData;
				const auto& blockchain = appContext.getCommunityContext(tx.txCommunityIdIndex).getBlockchain();

				auto firstTransaction = blockchain->findOne(CompactFilter::firstTransaction());
				assert(firstTransaction->isCommunityRoot());

				vector<uint32_t> expectedSigner = {
					firstTransaction->getCommunityRootPublicKey().publicKeyIndex,
					registerAddress.userPublicKeyIndex,
					registerAddress.accountPublicKeyIndex
				};
				for (const auto& sigPair : coldData->signatureMap) {
					auto publicKeyIndex = publicKeyIdDict.getIndexForData(sigPair.first);
					for (auto it = expectedSigner.begin(); it != expectedSigner.end(); ++it) {
						if (publicKeyIndex == *it) {
							it = expectedSigner.erase(it);
							break;
						}
					}
				}
				if (expectedSigner.size()) {
					return {
						.type = ErrorType::Missing_Sign,
						.message = "register address expected to be signed by user public key, account public key and community root public key"
					};
				}
			}
			return { .type = ErrorType::Success };
		}

		Error validateRegisterAddress(const ConfirmedGradidoTx& tx, const AppContext& appContext, Options options)
		{
			if (!tx.isRegisterAddress()) {
				throw GradidoNodeInvalidDataException("called validateRegisterAddress with not register address typed tx");
			}
			if (!appContext.getCommunityContext(tx.txCommunityIdIndex).getBlockchain().get()) {
				return {
					.type = ErrorType::Missing_Blockchain,
					.message = "missing blockchain for community " + to_string(tx.txCommunityIdIndex)
				};
			}
			Error result;
			if ((Type::SINGLE & options.type) == Type::SINGLE) {
				result = validateSingleRegisterAddress(tx, appContext, options);
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