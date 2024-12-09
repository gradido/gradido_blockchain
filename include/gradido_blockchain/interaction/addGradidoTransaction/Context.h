#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_ADD_GRADIDO_TRANSACTION_CONTEXT_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_ADD_GRADIDO_TRANSACTION_CONTEXT_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/types.h"
#include "gradido_blockchain/memory/Block.h"

// #include <memory>

namespace gradido {
    namespace blockchain {
        class Abstract;
    }
    namespace data {
        class GradidoTransaction;
    }
    namespace interaction {
        namespace addGradidoTransaction {
            class GRADIDOBLOCKCHAIN_EXPORT Context 
            {
            public:
                Context(std::shared_ptr<const blockchain::Abstract> blockchain)
                    : mBlockchain(blockchain) {}

                virtual ~Context() {}
                bool run(std::shared_ptr<const data::GradidoTransaction> gradidoTransaction, memory::ConstBlockPtr messageId, Timepoint confirmedAt);
                
            protected:
                std::shared_ptr<const blockchain::Abstract> mBlockchain;
            };
        }
    }
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_ADD_GRADIDO_TRANSACTION_CONTEXT_H