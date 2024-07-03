#include "gradido_blockchain/interaction/deserialize/RegisterAddressRole.h"
#include "gradido_blockchain/interaction/deserialize/Exceptions.h"

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
            {
                if (!registerAddressMessage["address_type"_f].has_value()) {
                    throw MissingMemberException("missing member on deserialize register address transaction", "address_type");
                }
				if (!registerAddressMessage["derivation_index"_f].has_value()) {
					throw MissingMemberException("missing member on deserialize register address transaction", "derivation_index");
				}
                mRegisterAddress = std::make_unique<data::RegisterAddress>(
                    registerAddressMessage["address_type"_f].value(),
                    registerAddressMessage["derivation_index"_f].value()
                );
                if (registerAddressMessage["user_pubkey"_f].has_value()) {
                    mRegisterAddress->userPubkey = std::make_shared<memory::Block>(registerAddressMessage["user_pubkey"_f].value());
                }
                if (registerAddressMessage["name_hash"_f].has_value()) {
                    mRegisterAddress->nameHash = std::make_shared<memory::Block>(registerAddressMessage["name_hash"_f].value());
                }
                if (registerAddressMessage["account_pubkey"_f].has_value()) {
                    mRegisterAddress->accountPubkey = std::make_shared<memory::Block>(registerAddressMessage["account_pubkey"_f].value());
                }
					
                    
            }
        }
    }
}