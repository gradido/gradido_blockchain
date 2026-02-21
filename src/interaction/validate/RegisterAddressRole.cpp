#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/FilterBuilder.h"
#include "gradido_blockchain/blockchain/SearchDirection.h"
#include "gradido_blockchain/blockchain/TransactionEntry.h"
#include "gradido_blockchain/data/adapter/publicKey.h"
#include "gradido_blockchain/data/AddressType.h"
#include "gradido_blockchain/data/compact/PublicKeyIndex.h"
#include "gradido_blockchain/data/compact/RegisterAddressTx.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/data/RegisterAddress.h"
#include "gradido_blockchain/data/SignatureMap.h"
#include "gradido_blockchain/data/TransactionType.h"
#include "gradido_blockchain/interaction/validate/RegisterAddressRole.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/memory/Block.h"

#include "date/date.h"
#include "magic_enum/magic_enum.hpp"

#include <memory>
#include <optional>
#include <string>

using memory::Block, memory::ConstBlockPtr;
using std::optional, std::nullopt;
using std::shared_ptr, std::make_shared;
using std::string, std::to_string;

using namespace magic_enum;

namespace gradido {
	using blockchain::Filter, blockchain::FilterBuilder, blockchain::SearchDirection, blockchain::TransactionEntry;
	using data::adapter::toConstBlockPtr;
	using data::compact::PublicKeyIndex;
	using data::AddressType, data::RegisterAddress, data::SignatureMap, data::TransactionType;
	namespace interaction {
		namespace validate {

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
					throw TransactionValidationRequiredSignMissingException({ { communityRoot->publicKeyIndex } });
				}
			}
		}
	}
}