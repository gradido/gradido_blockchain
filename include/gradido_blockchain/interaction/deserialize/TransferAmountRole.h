#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_TRANSFER_AMOUNT_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_TRANSFER_AMOUNT_ROLE_H

#include "Protopuf.h"
#include "gradido_blockchain/data/Protocol.h"

namespace gradido {
    namespace interaction {
        namespace deserialize {
            class TransferAmountRole
            {
            public:
                TransferAmountRole(const TransferAmountMessage& transferAmount);
                inline operator const data::TransferAmount&() const { return *mTransferAmount.get(); }
                inline const data::TransferAmount& data() const { return *mTransferAmount.get(); }
            protected:
                std::unique_ptr<data::TransferAmount> mTransferAmount;
            };
        }
    }
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_TRANSFER_AMOUNT_ROLE_H