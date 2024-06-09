#include "gradido_blockchain/v3_3/interaction/deserialize/RegisterAddressRole.h"

namespace gradido {
    namespace v3_3 {
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
                    : mRegisterAddress(
                        registerAddressMessage["address_type"_f].value(),
                        registerAddressMessage["derivation_index"_f].value()
                    )
                {
                    if (registerAddressMessage["user_pubkey"_f].has_value()) {
                        mRegisterAddress.userPubkey = std::make_shared<memory::Block>(registerAddressMessage["user_pubkey"_f].value());
                    }
                    if (registerAddressMessage["name_hash"_f].has_value()) {
                        mRegisterAddress.nameHash = std::make_shared<memory::Block>(registerAddressMessage["name_hash"_f].value());
                    }
                    if (registerAddressMessage["account_pubkey"_f].has_value()) {
                        mRegisterAddress.accountPubkey = std::make_shared<memory::Block>(registerAddressMessage["account_pubkey"_f].value());
                    }
                    
                }
            }
        }
    }
}