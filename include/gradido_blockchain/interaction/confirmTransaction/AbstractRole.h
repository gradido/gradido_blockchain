#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CONFIRM_TRANSACTION_ABSTRACT_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CONFIRM_TRANSACTION_ABSTRACT_ROLE_H

#include "gradido_blockchain/blockchain/TransactionRelationType.h"
#include "gradido_blockchain/GradidoUnit.h"
#include "gradido_blockchain/interaction/validate/Type.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/types.h"

namespace gradido {
    namespace blockchain {
        class Abstract;
        class TransactionEntry;
    }
    namespace data {
        class GradidoTransaction;
        class ConfirmedTransaction;
        class AccountBalance;
    }

    namespace interaction {
        namespace confirmTransaction {
            
            class GRADIDOBLOCKCHAIN_EXPORT AbstractRole
            {
            public:
                AbstractRole(
                    std::shared_ptr<const data::GradidoTransaction> gradidoTransaction,
                    memory::ConstBlockPtr messageId, 
                    Timepoint confirmedAt,
                    std::shared_ptr<blockchain::Abstract> blockchain
                );
                virtual ~AbstractRole();

                inline std::shared_ptr<const data::GradidoTransaction> getGradidoTransaction() const { return mGradidoTransaction; }
                inline Timepoint getConfirmedAt() const { return mConfirmedAt; }

                std::shared_ptr<const data::ConfirmedTransaction> createConfirmedTransaction(uint64_t id) const;

                //! will be called before main validation
                virtual void runPreValidate(std::shared_ptr<const data::ConfirmedTransaction> confirmedTransaction, std::shared_ptr<blockchain::Abstract> blockchain) {};
                virtual validate::Type getValidationType() const = 0;

                //! will be called after transaction was pushed to blockchain
                virtual void runPastAddToBlockchain(
                    std::shared_ptr<const data::ConfirmedTransaction> confirmedTransaction,
                    std::shared_ptr<blockchain::Abstract> blockchain
                ) const {};

                virtual std::vector<data::AccountBalance> calculateAccountBalances(uint64_t maxTransactionNr) const = 0;

            protected:
                data::AccountBalance calculateAccountBalance(memory::ConstBlockPtr publicKey, uint64_t maxTransactionNr, GradidoUnit amount) const;

                std::shared_ptr<const data::GradidoTransaction> mGradidoTransaction;
                memory::ConstBlockPtr mMessageId;
                Timepoint mConfirmedAt;
                std::shared_ptr<blockchain::Abstract> mBlockchain;
            };
        }
    }
}

#endif // __GRADIDO_BLOCKCHAIN_INTERACTION_CONFIRM_TRANSACTION_ABSTRACT_ROLE_H