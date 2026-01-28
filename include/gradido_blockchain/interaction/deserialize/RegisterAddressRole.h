#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_REGISTER_ADDRESS_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_REGISTER_ADDRESS_ROLE_H

#include "Protopuf.h"

namespace gradido {
    namespace data::compact {
        class RegisterAddressTx;
    }
    namespace interaction {
        namespace deserialize {
            class RegisterAddressRole
            {
            public:
                RegisterAddressRole(const RegisterAddressMessage& registerAddressMessage);
                ~RegisterAddressRole() {}
                data::compact::RegisterAddressTx run(uint32_t communityIdIndex) const;
            protected:
                const RegisterAddressMessage& mRegisterAddressMessage;
            };
        }
    }
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_REGISTER_ADDRESS_ROLE_H