#ifndef __GRADIDO_BLOCKCHAIN_v3_3_INTERACTION_DESERIALIZE_REGISTER_ADDRESS_ROLE_H
#define __GRADIDO_BLOCKCHAIN_v3_3_INTERACTION_DESERIALIZE_REGISTER_ADDRESS_ROLE_H

#include "Protopuf.h"
#include "gradido_blockchain/v3_3/data/Protocol.h"

namespace gradido {
    namespace v3_3 {
        namespace interaction {
            namespace deserialize {
                class RegisterAddressRole
                {
                public:
                    RegisterAddressRole(const RegisterAddressMessage& registerAddressMessage);
                    inline operator const data::RegisterAddress&() const { return *mRegisterAddress.get(); }
                    inline const data::RegisterAddress& data() const { return *mRegisterAddress.get(); }
                protected:
                    std::unique_ptr<data::RegisterAddress> mRegisterAddress;
                };
            }
        }
    }
}

#endif //__GRADIDO_BLOCKCHAIN_v3_3_INTERACTION_DESERIALIZE_REGISTER_ADDRESS_ROLE_H