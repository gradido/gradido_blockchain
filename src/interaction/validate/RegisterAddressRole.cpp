#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/FilterBuilder.h"
#include "gradido_blockchain/interaction/validate/RegisterAddressRole.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"

#include "date/date.h"
#include "magic_enum/magic_enum.hpp"

using namespace magic_enum;

namespace gradido {
	namespace interaction {
		namespace validate {

			RegisterAddressRole::RegisterAddressRole(std::shared_ptr<const data::RegisterAddress> registerAddress)
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

			void RegisterAddressRole::run(
				Type type,
				std::shared_ptr<blockchain::Abstract> blockchain,
				std::shared_ptr<const data::ConfirmedTransaction> senderPreviousConfirmedTransaction,
				std::shared_ptr<const data::ConfirmedTransaction> recipientPreviousConfirmedTransaction
			) {
				auto addressType = mRegisterAddress->getAddressType();
				auto accountPubkey = mRegisterAddress->getAccountPublicKey();
				auto userPubkey = mRegisterAddress->getUserPublicKey();

				if (data::AddressType::COMMUNITY_PROJECT == addressType ||
					data::AddressType::COMMUNITY_HUMAN == addressType) {
				}
				if ((type & Type::SINGLE) == Type::SINGLE)
				{
					if (data::AddressType::COMMUNITY_GMW == addressType ||
						data::AddressType::COMMUNITY_AUF == addressType ||
						data::AddressType::NONE == addressType) {
						throw WrongAddressTypeException(
							"register address transaction not allowed with community auf or gmw account or None",
							addressType, 
							mRegisterAddress->getUserPublicKey()
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
					assert(blockchain);
					blockchain::FilterBuilder filterBuilder;

					std::shared_ptr<const blockchain::TransactionEntry> transactionWithSameAddress;
					if (!senderPreviousConfirmedTransaction) {
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
						transactionWithSameAddress = blockchain->findOne(
							filterBuilder
							.setInvolvedPublicKey(userPubkey)
							.setMaxTransactionNr(senderPreviousConfirmedTransaction->getId())
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
						transactionWithSameAddress = blockchain->findOne(
							filterBuilder
							.setInvolvedPublicKey(userPubkey)
							.setMaxTransactionNr(senderPreviousConfirmedTransaction->getId())
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
				for (auto& signPair : signPairs) {
					if(signPair.getPublicKey()->isTheSame(mRegisterAddress->getAccountPublicKey()) ||
					   signPair.getPublicKey()->isTheSame(mRegisterAddress->getUserPublicKey())) {
						continue;
					}
					if (signPair.getPublicKey()->isTheSame(communityRoot->getTransactionBody()->getCommunityRoot()->getPublicKey())) {
						foundCommunityRootSigner = true;
						break;
					}
				}
				if (!foundCommunityRootSigner) {
					throw TransactionValidationRequiredSignMissingException({ communityRoot->getTransactionBody()->getCommunityRoot()->getPublicKey()});
				}
			}
		}
	}
}