#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CONFIRM_TRANSACTION_CONTEXT_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CONFIRM_TRANSACTION_CONTEXT_H

#include "AbstractRole.h"

namespace gradido {
    namespace blockchain {
        class Abstract;
    }
    namespace data {
        class EventTriggeredTransaction;
        class Timestamp;
    }
    namespace interaction {
        namespace confirmTransaction {            

            /*!
            * @author einhornimmond
            * @date 02.01.2025
            * @brief create confirmed Transaction, use roles to run transaction type specific code
            * first call createRole, then use returned role with run or create your own role and after adding data::ConfirmedTransaction,
            * please call runPastAddToBlockchain from Role!
            */
            class GRADIDOBLOCKCHAIN_EXPORT Context 
            {
            public:
                Context(std::shared_ptr<blockchain::Abstract> blockchain)
                    : mBlockchain(blockchain), mDisableVerify(false), mDisableRunningHashTest(false) {}

                virtual ~Context() {}

                virtual std::shared_ptr<AbstractRole> createRole(
                    std::shared_ptr<const data::GradidoTransaction> gradidoTransaction,
                    const data::LedgerAnchor& ledgerAnchor,
                    data::Timestamp confirmedAt
                ) const;

                std::shared_ptr<const data::ConfirmedTransaction> run(std::shared_ptr<AbstractRole> role);
                inline void disableVerify() { mDisableVerify = true; }
                inline void disableRunningHashTest() { mDisableRunningHashTest = true; }
                
            protected:
                bool processTransactionTrigger(data::Timestamp endDate);
                std::shared_ptr<blockchain::Abstract> mBlockchain;
                bool mDisableVerify;
                bool mDisableRunningHashTest;

            };
        }
    }
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_CONFIRM_TRANSACTION_CONTEXT_H