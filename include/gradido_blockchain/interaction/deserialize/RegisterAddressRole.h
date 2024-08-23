#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_REGISTER_ADDRESS_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_REGISTER_ADDRESS_ROLE_H

#include "Protopuf.h"
#include "gradido_blockchain/data/RegisterAddress.h"

namespace gradido {
    namespace interaction {
        namespace deserialize {
            class RegisterAddressRole
            {
            public:
                RegisterAddressRole(const RegisterAddressMessage& registerAddressMessage);
                std::unique_ptr<data::RegisterAddress> run() const;
            protected:
                const RegisterAddressMessage& mRegisterAddressMessage;
            };
        }
    }
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_REGISTER_ADDRESS_ROLE_H