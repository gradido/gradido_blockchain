#include "gradido_blockchain/v3_3/interaction/validate/RegisterAddressRole.h"
#include "gradido_blockchain/v3_3/interaction/validate/Exceptions.h"

#include "date/date.h"
#include "magic_enum/magic_enum.hpp"

using namespace magic_enum;

namespace gradido {
	namespace v3_3 {
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
					const std::string& communityId,
					std::shared_ptr<AbstractBlockchainProvider> blockchainProvider,
					data::ConfirmedTransactionPtr senderPreviousConfirmedTransaction,
					data::ConfirmedTransactionPtr recipientPreviousConfirmedTransaction
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

						std::shared_ptr<TransactionEntry> lastTransaction;
						if (data::AddressType::SUBACCOUNT == mRegisterAddress.addressType) {
							lastTransaction = blockchain->findLastTransactionForAddress(mRegisterAddress.userPubkey, "", senderPreviousConfirmedTransaction->id);
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
						lastTransaction = blockchain->findLastTransactionForAddress(address, "", senderPreviousConfirmedTransaction->id);
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
}