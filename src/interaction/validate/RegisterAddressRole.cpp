#include "gradido_blockchain/interaction/validate/RegisterAddressRole.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/blockchain/FilterBuilder.h"

#include "date/date.h"
#include "magic_enum/magic_enum.hpp"

using namespace magic_enum;

namespace gradido {
	namespace interaction {
		namespace validate {

			RegisterAddressRole::RegisterAddressRole(std::shared_ptr<const data::RegisterAddress> registerAddress)
				: mRegisterAddress(registerAddress) 
			{
				if (registerAddress->accountPubkey) {
					mRequiredSignPublicKeys.push_back(registerAddress->accountPubkey);
				}
				else if (registerAddress->userPubkey) {
					mRequiredSignPublicKeys.push_back(registerAddress->userPubkey);
				}
				mMinSignatureCount = 2;
			}

			void RegisterAddressRole::run(
				Type type,
				std::string_view communityId,
				blockchain::AbstractProvider* blockchainProvider,
				data::ConstConfirmedTransactionPtr senderPreviousConfirmedTransaction,
				data::ConstConfirmedTransactionPtr recipientPreviousConfirmedTransaction
			) {
				if (data::AddressType::COMMUNITY_PROJECT == mRegisterAddress->addressType ||
					data::AddressType::COMMUNITY_HUMAN == mRegisterAddress->addressType) {
				}
				if ((type & Type::SINGLE) == Type::SINGLE) {
					if (data::AddressType::COMMUNITY_GMW == mRegisterAddress->addressType ||
						data::AddressType::COMMUNITY_AUF == mRegisterAddress->addressType ||
						data::AddressType::NONE == mRegisterAddress->addressType) {
						throw WrongAddressTypeException(
							"register address transaction not allowed with community auf or gmw account or None",
							mRegisterAddress->addressType, mRegisterAddress->userPubkey
						);
					}
					auto& accountPubkey = mRegisterAddress->accountPubkey;
					if (accountPubkey) {
						validateEd25519PublicKey(accountPubkey, "accountPubkey");
					}
					auto& userPubkey = mRegisterAddress->userPubkey;
					if (userPubkey) {
						validateEd25519PublicKey(mRegisterAddress->userPubkey, "userPubkey");
					}
					if (accountPubkey && userPubkey && accountPubkey->isTheSame(userPubkey)) {
						throw TransactionValidationException("accountPubkey and userPubkey are the same");
					}
					if (!accountPubkey && !userPubkey) {
						throw TransactionValidationException("accountPubkey and userPubkey are both empty, at least one is needed");
					}
				}

				if ((type & Type::ACCOUNT) == Type::ACCOUNT) {
					assert(blockchainProvider);
					auto blockchain = blockchainProvider->findBlockchain(communityId);
					assert(blockchain);
					blockchain::FilterBuilder filterBuilder;

					std::shared_ptr<blockchain::TransactionEntry> transactionWithSameAddress;
					if (data::AddressType::SUBACCOUNT == mRegisterAddress->addressType) {
						transactionWithSameAddress = blockchain->findOne(
							filterBuilder
							.setInvolvedPublicKey(mRegisterAddress->userPubkey)
							.setMaxTransactionNr(senderPreviousConfirmedTransaction->id)
							.setSearchDirection(blockchain::SearchDirection::DESC)
							.build()
						);
						if (!transactionWithSameAddress) {
							throw AddressAlreadyExistException(
								"cannot register sub address because user is missing",
								mRegisterAddress->userPubkey->convertToHex(),
								mRegisterAddress->addressType
							);
						}
						transactionWithSameAddress.reset();
					}

					memory::ConstBlockPtr address;

					switch (mRegisterAddress->addressType) {
					case data::AddressType::COMMUNITY_HUMAN:
					case data::AddressType::COMMUNITY_PROJECT:
					case data::AddressType::CRYPTO_ACCOUNT:
						address = mRegisterAddress->userPubkey;
						break;
					case data::AddressType::SUBACCOUNT:
						address = mRegisterAddress->accountPubkey;
						break;
					}
					if (!address) {
						throw GradidoUnhandledEnum(
							"register address has invalid type for account validation",
							enum_type_name<decltype(mRegisterAddress->addressType)>().data(),
							enum_name(mRegisterAddress->addressType).data()
						);
					}
					transactionWithSameAddress = blockchain->findOne(
						filterBuilder
						.setInvolvedPublicKey(address)
						.setMaxTransactionNr(senderPreviousConfirmedTransaction->id)
						.setSearchDirection(blockchain::SearchDirection::DESC)
						.setPagination({1})
						.build()
					);
					if (transactionWithSameAddress) {
						throw AddressAlreadyExistException(
							"cannot register address because it already exist",
							address->convertToHex(),
							mRegisterAddress->addressType
						);
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
				auto& signPairs = signatureMap.signaturePairs;

				// get community root transaction
				blockchain::Filter filter;
				filter.transactionType = data::TransactionType::COMMUNITY_ROOT;
				filter.searchDirection = blockchain::SearchDirection::ASC;
				auto communityRoot = blockchain->findOne(filter);
				if (!communityRoot) {
					throw BlockchainOrderException("cannot finde community root transaction before register address");
				}
				bool foundCommunityRootSigner = false;

				// check for account type
				for (auto& signPair : signPairs) {
					if(signPair.pubkey->isTheSame(mRegisterAddress->accountPubkey) ||
					   signPair.pubkey->isTheSame(mRegisterAddress->userPubkey)) {
						continue;
					}
					if (signPair.pubkey->isTheSame(communityRoot->getTransactionBody()->communityRoot->pubkey)) {
						foundCommunityRootSigner = true;
						break;
					}
				}
				if (!foundCommunityRootSigner) {
					throw TransactionValidationRequiredSignMissingException({ communityRoot->getTransactionBody()->communityRoot->pubkey });
				}
			}
		}
	}
}