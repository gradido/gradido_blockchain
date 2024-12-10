#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_ADD_GRADIDO_TRANSACTION_CONTEXT_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_ADD_GRADIDO_TRANSACTION_CONTEXT_H

#include "AbstractRole.h"
#include "ResultType.h"

namespace gradido {
    namespace blockchain {
        class Abstract;
    }
    namespace interaction {
        namespace addGradidoTransaction {            

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

                //! check if transaction already exist in blockchain
                //! \return false if transaction not exist
                virtual bool isExisting(std::shared_ptr<const AbstractRole> role) const = 0;               

                //! finally, add transaction to blockchain
                virtual void addToBlockchain(std::shared_ptr<const data::ConfirmedTransaction> confirmedTransaction) = 0;

                //! run after addToBlockchain and after call to AbstractRole::runPastAddToBlockchain
                virtual void finalize() {};
                
            protected:
                std::shared_ptr<blockchain::Abstract> mBlockchain;
            };
        }
    }
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_ADD_GRADIDO_TRANSACTION_CONTEXT_H