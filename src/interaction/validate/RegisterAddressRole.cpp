#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/FilterBuilder.h"
#include "gradido_blockchain/data/AddressType.h"
#include "gradido_blockchain/data/RegisterAddress.h"
#include "gradido_blockchain/interaction/validate/RegisterAddressRole.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/memory/Block.h"

#include "date/date.h"
#include "magic_enum/magic_enum.hpp"

#include <memory>
#include <string>

using memory::Block, memory::ConstBlockPtr;
using std::shared_ptr, std::make_shared;
using std::string;

using namespace magic_enum;

namespace gradido {
	using data::AddressType, data::RegisterAddress;
	namespace interaction {
		namespace validate {

			RegisterAddressRole::RegisterAddressRole(shared_ptr<const RegisterAddress> registerAddress)
				: mRegisterAddress(registerAddress) 
			{
				assert(registerAddress);
				if (registerAddress->getAccountPublicKey()) {
					mRequiredSignPublicKeys.push_back(registerAddress->getAccountPublicKey());
				}
				else if (registerAddress->getUserPublicKey()) {
					mRequiredSignPublicKeys.push_back(registerAddress->getUserPublicKey());
				}
				mMinSignatureCount = 2;
			}

			void RegisterAddressRole::run(Type type, ContextData& c)
			{
				auto addressType = mRegisterAddress->getAddressType();
				auto accountPubkey = mRegisterAddress->getAccountPublicKey();
				auto userPubkey = mRegisterAddress->getUserPublicKey();

				if (AddressType::COMMUNITY_PROJECT == addressType ||
					AddressType::COMMUNITY_HUMAN == addressType) {
				}
				if ((type & Type::SINGLE) == Type::SINGLE)
				{
					if (AddressType::COMMUNITY_GMW == addressType ||
						AddressType::COMMUNITY_AUF == addressType ||
						AddressType::NONE == addressType) 
					{
						std::optional<uint32_t> communityIdIndex = std::nullopt;
						if (c.senderBlockchain) {
							communityIdIndex = c.senderBlockchain->getCommunityIdIndex();
						}
						throw WrongAddressTypeException(
							"register address transaction not allowed with community auf or gmw account or None",
							addressType,
							mRegisterAddress->getUserPublicKey(),
							communityIdIndex
						);
					}

					if (accountPubkey) {
						validateEd25519PublicKey(accountPubkey, "accountPubkey");
					}
					if (userPubkey) {
						validateEd25519PublicKey(userPubkey, "userPubkey");
					}
					if (accountPubkey && userPubkey && accountPubkey->isTheSame(userPubkey)) {
						throw TransactionValidationException("accountPubkey and userPubkey are the same");
					}
					if (!accountPubkey && !userPubkey) {
						throw TransactionValidationException("accountPubkey and userPubkey are both empty, at least one is needed");
					}
				}

				if ((type & Type::ACCOUNT) == Type::ACCOUNT)
				{
					assert(c.senderBlockchain);
					blockchain::FilterBuilder filterBuilder;

					std::shared_ptr<const blockchain::TransactionEntry> transactionWithSameAddress;
					if (!c.senderPreviousConfirmedTransaction) {
						throw GradidoNullPointerException(
							"missing previous confirmed transaction for sender in interaction::validate RegisterAddress Type::ACCOUNT",
							"data::ConstConfirmedTransactionPtr",
							__FUNCTION__
						);
					}
					if (data::AddressType::SUBACCOUNT == addressType) {
						if (!userPubkey) {
							throw GradidoNullPointerException(
								"missing user pubkey for subaccount",
								"memory::ConstBlockPtr",
								__FUNCTION__
							);
						}
						transactionWithSameAddress = c.senderBlockchain->findOne(
							filterBuilder
							.setInvolvedPublicKey(userPubkey)
							.setMaxTransactionNr(c.senderPreviousConfirmedTransaction->getId())
							.setSearchDirection(blockchain::SearchDirection::DESC)
							.build()
						);
						if (!transactionWithSameAddress) {
							throw AddressAlreadyExistException(
								"cannot register sub address because user is missing",
								userPubkey->convertToHex(),
								addressType
							);
						}
						transactionWithSameAddress.reset();
					}
					else {
						if (!userPubkey) {
							throw GradidoUnhandledEnum(
								"register address has invalid type for account validation",
								enum_type_name<decltype(addressType)>().data(),
								enum_name(addressType).data()
							);
						}
						transactionWithSameAddress = c.senderBlockchain->findOne(
							filterBuilder
							.setInvolvedPublicKey(userPubkey)
							.setMaxTransactionNr(c.senderPreviousConfirmedTransaction->getId())
							.setTransactionType(data::TransactionType::REGISTER_ADDRESS)
							.setSearchDirection(blockchain::SearchDirection::DESC)
							.setPagination({ 1 })
							.build()
						);
						if (transactionWithSameAddress) {
							if (
								(accountPubkey && transactionWithSameAddress->getTransactionBody()->isInvolved(*accountPubkey)) ||
								(userPubkey && transactionWithSameAddress->getTransactionBody()->isInvolved(*userPubkey))
								) {
								throw AddressAlreadyExistException(
									"cannot register address because it already exist",
									userPubkey->convertToHex(),
									addressType
								);
							}
						}
					}
				}
			}

			void RegisterAddressRole::checkRequiredSignatures(
				const data::SignatureMap& signatureMap,
				std::shared_ptr<blockchain::Abstract> blockchain /*  = nullptr*/
			) const
			{
				AbstractRole::checkRequiredSignatures(signatureMap, blockchain);
				if (!blockchain) return;
				auto& signPairs = signatureMap.getSignaturePairs();

				// get community root transaction
				blockchain::Filter filter;
				filter.transactionType = data::TransactionType::COMMUNITY_ROOT;
				filter.searchDirection = blockchain::SearchDirection::ASC;
				auto communityRoot = blockchain->findOne(filter);
				if (!communityRoot) {
					throw BlockchainOrderException("cannot find community root transaction before register address");
				}
				bool foundCommunityRootSigner = false;

				// check for account type
				auto communityRootPublicKey = g_appContext->getPublicKey(communityRoot->getTransactionBody()->getCommunityRoot().value().publicKeyIndex).value();
				for (auto& signPair : signPairs) {
					if(signPair.getPublicKey()->isTheSame(mRegisterAddress->getAccountPublicKey()) ||
					   signPair.getPublicKey()->isTheSame(mRegisterAddress->getUserPublicKey())) {
						continue;
					}
					if (communityRootPublicKey.isTheSame(signPair.getPublicKey()->data())) {
						foundCommunityRootSigner = true;
						break;
					}
				}
				if (!foundCommunityRootSigner) {
					throw TransactionValidationRequiredSignMissingException({ make_shared<const Block>(communityRootPublicKey)});
				}
			}
		}
	}
}