#ifndef __GRADIDO_BLOCKCHAIN_v3_3_INTERACTION_DESERIALIZE_TRANSFER_AMOUNT_ROLE_H
#define __GRADIDO_BLOCKCHAIN_v3_3_INTERACTION_DESERIALIZE_TRANSFER_AMOUNT_ROLE_H

#include "Protopuf.h"
#include "gradido_blockchain/v3_3/data/Protocol.h"

namespace gradido {
    namespace v3_3 {
        namespace interaction {
            namespace deserialize {
                class TransferAmountRole
                {
                public:
                    TransferAmountRole(const TransferAmountMessage& transferAmount);
                    inline operator data::TransferAmount() const { return mTransferAmount; }
                    inline data::TransferAmount data() const { return mTransferAmount; }
                protected:
                    data::TransferAmount mTransferAmount;
                };
            }
        }
    }
}

#endif //__GRADIDO_BLOCKCHAIN_v3_3_INTERACTION_DESERIALIZE_TRANSFER_AMOUNT_ROLE_H