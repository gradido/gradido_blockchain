#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_GRADIDO_TRANSFER_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_GRADIDO_TRANSFER_ROLE_H

#include "Protopuf.h"

namespace gradido {
    namespace data {
        class GradidoTransfer;
    }
    namespace interaction {
        namespace deserialize {
            class GradidoTransferRole
            {
            public:
                GradidoTransferRole(const GradidoTransferMessage& gradidoTransfer);
                ~GradidoTransferRole() {}
                std::unique_ptr<data::GradidoTransfer> run() const;
            protected:
                const GradidoTransferMessage& mGradidoTransferMessage;
            };
        }
    }
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_GRADIDO_TRANSFER_ROLE_H