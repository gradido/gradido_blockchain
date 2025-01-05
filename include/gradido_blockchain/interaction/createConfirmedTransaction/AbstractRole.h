#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CREATE_CONFIRMED_TRANSACTION_ABSTRACT_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CREATE_CONFIRMED_TRANSACTION_ABSTRACT_ROLE_H

#include "gradido_blockchain/GradidoUnit.h"
#include "gradido_blockchain/interaction/validate/Type.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/types.h"

namespace gradido {
    namespace blockchain {
        class Abstract;
    }
    namespace data {
        class GradidoTransaction;
        class ConfirmedTransaction;
    }

    namespace interaction {
        namespace createConfirmedTransaction {
            
            class GRADIDOBLOCKCHAIN_EXPORT AbstractRole
            {
            public:
                AbstractRole(std::shared_ptr<data::GradidoTransaction> gradidoTransaction, memory::ConstBlockPtr messageId, Timepoint confirmedAt);
                virtual ~AbstractRole();

                inline std::shared_ptr<const data::GradidoTransaction> getGradidoTransaction() const { return mGradidoTransaction; }
                inline Timepoint getConfirmedAt() const { return mConfirmedAt; }

                virtual GradidoUnit calculateFinalBalance(uint64_t id, const blockchain::Abstract& blockchain) const;

                std::shared_ptr<const data::ConfirmedTransaction> createConfirmedTransaction(
                    uint64_t id, 
                    std::shared_ptr<const data::ConfirmedTransaction> lastConfirmedTransaction,
                    const blockchain::Abstract& blockchain
                ) const;

                //! will be called before main validation
                virtual void runPreValidate(std::shared_ptr<const data::ConfirmedTransaction> confirmedTransaction, std::shared_ptr<blockchain::Abstract> blockchain) {};
                virtual validate::Type getValidationType() const = 0;

                //! will be called after transaction was pushed to blockchain
                virtual void runPastAddToBlockchain(
                    std::shared_ptr<const data::ConfirmedTransaction> confirmedTransaction,
                    std::shared_ptr<blockchain::Abstract> blockchain
                ) const {};

            protected:
                std::shared_ptr<data::GradidoTransaction> mGradidoTransaction;
                memory::ConstBlockPtr mMessageId;
                Timepoint mConfirmedAt;
            };
        }
    }
}

#endif // __GRADIDO_BLOCKCHAIN_INTERACTION_CREATE_CONFIRMED_TRANSACTION_ABSTRACT_ROLE_H