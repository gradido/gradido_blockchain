#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CREATE_CONFIRMED_TRANSACTION_CONTEXT_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CREATE_CONFIRMED_TRANSACTION_CONTEXT_H

#include "ResultType.h"
#include "AbstractRole.h"

namespace gradido {
    namespace blockchain {
        class Abstract;
    }
    namespace data {
        class EventTriggeredTransaction;
    }
    namespace interaction {
        namespace createConfirmedTransaction {            

            class GRADIDOBLOCKCHAIN_EXPORT Context 
            {
            public:
                Context(std::shared_ptr<blockchain::Abstract> blockchain)
                    : mBlockchain(blockchain) {}

                virtual ~Context() {}

                //! can be overloaded for using custom roles
                virtual std::shared_ptr<AbstractRole> createRole(
                    std::shared_ptr<data::GradidoTransaction> gradidoTransaction,
                    memory::ConstBlockPtr messageId,
                    Timepoint confirmedAt
                ) const;

                ResultType run(std::shared_ptr<AbstractRole> role);            
                
            protected:
                std::shared_ptr<blockchain::Abstract> mBlockchain;
            };
        }
    }
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_CREATE_CONFIRMED_TRANSACTION_CONTEXT_H