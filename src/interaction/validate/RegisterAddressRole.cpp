#include "gradido_blockchain/interaction/validate/RegisterAddressRole.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/blockchain/FilterBuilder.h"

#include "date/date.h"
#include "magic_enum/magic_enum.hpp"

using namespace magic_enum;

namespace gradido {
	namespace interaction {
		namespace validate {

			RegisterAddressRole::RegisterAddressRole(const data::RegisterAddress& registerAddress)
				: mRegisterAddress(registerAddress) 
			{
				if (registerAddress.accountPubkey) {
					mRequiredSignPublicKeys.push_back(registerAddress.accountPubkey);
				}
				else if (registerAddress.userPubkey) {
					mRequiredSignPublicKeys.push_back(registerAddress.userPubkey);
				}
				mMinSignatureCount = 1;
			}

			void RegisterAddressRole::run(
				Type type,
				std::string_view communityId,
				blockchain::AbstractProvider* blockchainProvider,
				data::ConstConfirmedTransactionPtr senderPreviousConfirmedTransaction,
				data::ConstConfirmedTransactionPtr recipientPreviousConfirmedTransaction
			) {
				if ((type & Type::SINGLE) == Type::SINGLE) {
					if (mRegisterAddress.addressType == data::AddressType::COMMUNITY_GMW || 
						mRegisterAddress.addressType == data::AddressType::COMMUNITY_AUF) {
						throw WrongAddressTypeException(
							"register address transaction not allowed with community auf or gmw account",
							mRegisterAddress.addressType, mRegisterAddress.userPubkey
						);
					}
				}

				if ((type & Type::ACCOUNT) == Type::ACCOUNT) {
					assert(blockchainProvider);
					auto blockchain = blockchainProvider->findBlockchain(communityId);
					assert(blockchain);
					blockchain::FilterBuilder filterBuilder;

					std::shared_ptr<blockchain::TransactionEntry> lastTransaction;
					if (data::AddressType::SUBACCOUNT == mRegisterAddress.addressType) {
						lastTransaction = blockchain->findOne(
							filterBuilder
							.setInvolvedPublicKey(mRegisterAddress.userPubkey)
							.setMaxTransactionNr(senderPreviousConfirmedTransaction->id)
							.setSearchDirection(blockchain::SearchDirection::DESC)
							.build()
						);
						if (!lastTransaction) {
							throw AddressAlreadyExistException(
								"cannot register sub address because user is missing",
								mRegisterAddress.userPubkey->convertToHex(),
								mRegisterAddress.addressType
							);
						}
						lastTransaction.reset();
					}

					memory::ConstBlockPtr address;

					switch (mRegisterAddress.addressType) {
					case data::AddressType::COMMUNITY_HUMAN:
					case data::AddressType::COMMUNITY_PROJECT:
					case data::AddressType::CRYPTO_ACCOUNT:
						address = mRegisterAddress.userPubkey;
						break;
					case data::AddressType::SUBACCOUNT:
						address = mRegisterAddress.accountPubkey;
						break;
					}
					if (!address) {
						throw GradidoUnhandledEnum(
							"register address has invalid type for account validation",
							enum_type_name<decltype(mRegisterAddress.addressType)>().data(),
							enum_name(mRegisterAddress.addressType).data()
						);
					}
					lastTransaction = blockchain->findOne(
						filterBuilder
						.setInvolvedPublicKey(address)
						.setMaxTransactionNr(senderPreviousConfirmedTransaction->id)
						.setSearchDirection(blockchain::SearchDirection::DESC)
						.build()
					);
					if (lastTransaction) {
						throw AddressAlreadyExistException(
							"cannot register address because it already exist",
							address->convertToHex(),
							mRegisterAddress.addressType
						);
					}
				}
			}
		}
	}
}