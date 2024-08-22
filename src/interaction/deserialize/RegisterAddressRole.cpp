#include "gradido_blockchain/interaction/deserialize/RegisterAddressRole.h"
#include "gradido_blockchain/interaction/deserialize/Exceptions.h"

using namespace memory;
namespace gradido {
    namespace interaction {
        namespace deserialize {
            /*
            * 
                memory::ConstBlockPtr userPubkeyPtr,
				AddressType _addressType,
				uint32_t _derivationIndex = 1,
				memory::ConstBlockPtr nameHashPtr = nullptr,
				memory::ConstBlockPtr accountPubkeyPtr = nullptr
            */
            RegisterAddressRole::RegisterAddressRole(const RegisterAddressMessage& registerAddressMessage)
                : mRegisterAddressMessage(registerAddressMessage) 
            {

            }

            std::unique_ptr<data::RegisterAddress> RegisterAddressRole::run() const
            {
                if (!mRegisterAddressMessage["address_type"_f].has_value()) {
                    throw MissingMemberException("missing member on deserialize register address transaction", "address_type");
                }
				if (!mRegisterAddressMessage["derivation_index"_f].has_value()) {
					throw MissingMemberException("missing member on deserialize register address transaction", "derivation_index");
				}
                BlockPtr userPubkey = nullptr;
                BlockPtr nameHash = nullptr;
                BlockPtr accountPubkey = nullptr;
                if (mRegisterAddressMessage["user_pubkey"_f].has_value()) {
                    userPubkey = std::make_shared<memory::Block>(mRegisterAddressMessage["user_pubkey"_f].value());
                }
                if (mRegisterAddressMessage["name_hash"_f].has_value()) {
                    nameHash = std::make_shared<memory::Block>(mRegisterAddressMessage["name_hash"_f].value());
                }
                if (mRegisterAddressMessage["account_pubkey"_f].has_value()) {
                    accountPubkey = std::make_shared<memory::Block>(mRegisterAddressMessage["account_pubkey"_f].value());
                }

                return std::make_unique<data::RegisterAddress>(
                    mRegisterAddressMessage["address_type"_f].value(),
                    mRegisterAddressMessage["derivation_index"_f].value(),
                    userPubkey,
                    nameHash,
                    accountPubkey
                );  
            }
        }
    }
}