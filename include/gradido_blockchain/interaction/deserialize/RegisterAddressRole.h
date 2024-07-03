#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_REGISTER_ADDRESS_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_REGISTER_ADDRESS_ROLE_H

#include "Protopuf.h"
#include "gradido_blockchain/data/Protocol.h"

namespace gradido {
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

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_REGISTER_ADDRESS_ROLE_H