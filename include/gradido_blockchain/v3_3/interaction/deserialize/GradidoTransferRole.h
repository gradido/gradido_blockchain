#ifndef __GRADIDO_BLOCKCHAIN_v3_3_INTERACTION_DESERIALIZE_GRADIDO_TRANSFER_ROLE_H
#define __GRADIDO_BLOCKCHAIN_v3_3_INTERACTION_DESERIALIZE_GRADIDO_TRANSFER_ROLE_H

#include "Protopuf.h"
#include "gradido_blockchain/v3_3/data/Protocol.h"

namespace gradido {
    namespace v3_3 {
        namespace interaction {
            namespace deserialize {
                class GradidoTransferRole
                {
                public:
                    GradidoTransferRole(const GradidoTransferMessage& gradidoTransfer);
                    inline operator const data::GradidoTransfer&() const { return *mGradidoTransfer.get(); }
                    inline const data::GradidoTransfer& data() const { return *mGradidoTransfer.get(); }
                protected:
                    std::unique_ptr<data::GradidoTransfer> mGradidoTransfer;
                };
            }
        }
    }
}

#endif //__GRADIDO_BLOCKCHAIN_v3_3_INTERACTION_DESERIALIZE_GRADIDO_TRANSFER_ROLE_H