#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CONFIRM_TRANSACTION_ABSTRACT_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CONFIRM_TRANSACTION_ABSTRACT_ROLE_H

#include "gradido_blockchain/blockchain/TransactionRelationType.h"
#include "gradido_blockchain/data/AccountBalance.h"
#include "gradido_blockchain/data/Timestamp.h"
#include "gradido_blockchain/data/TransferAmount.h"
#include "gradido_blockchain/data/LedgerAnchor.h"
#include "gradido_blockchain/GradidoUnit.h"
#include "gradido_blockchain/interaction/validate/Type.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/types.h"
#include "gradido_blockchain_core/types/balance_derivation.h"

namespace gradido {
    namespace blockchain {
        class Abstract;
        class TransactionEntry;
    }
    namespace data {
        class ConfirmedTransaction;
        class GradidoTransaction;  
        namespace compact {
          struct PublicKeyIndex;
          struct AccountBalance;
        }
    }

    namespace interaction {
        namespace confirmTransaction {
            
            class GRADIDOBLOCKCHAIN_EXPORT AbstractRole
            {
            public:
                AbstractRole(
                    std::shared_ptr<const data::GradidoTransaction> gradidoTransaction,
                    const data::LedgerAnchor& ledgerAnchor,
                    data::Timestamp confirmedAt,
                    std::shared_ptr<blockchain::Abstract> blockchain
                );
                virtual ~AbstractRole();

                inline std::shared_ptr<const data::GradidoTransaction> getGradidoTransaction() const { return mGradidoTransaction; }
                data::Timestamp getConfirmedAt() const { return mConfirmedAt; }

                std::shared_ptr<const data::ConfirmedTransaction> createConfirmedTransaction(
                    uint64_t id,
                    std::shared_ptr<const data::ConfirmedTransaction> lastConfirmedTransaction
                );

                //! will be called before main validation
                virtual void runPreValidate(std::shared_ptr<const data::ConfirmedTransaction> confirmedTransaction, std::shared_ptr<blockchain::Abstract> blockchain) {};
                virtual validate::Type getValidationType() const = 0;

                //! will be called after transaction was pushed to blockchain
                virtual void runPastAddToBlockchain(
                    std::shared_ptr<const data::ConfirmedTransaction> confirmedTransaction,
                    std::shared_ptr<blockchain::Abstract> blockchain
                ) const {};

                virtual std::vector<data::AccountBalance> calculateAccountBalances(uint64_t maxTransactionNr) const = 0;
                //! set pre-calculated account balances
                //! will set mBalanceDerivationType to extern
                //! \param accountBalances move
                void setAccountBalances(std::vector<data::AccountBalance> accountBalances);

                inline bool isExternBalanceDerivationType() const { return GRDT_BALANCE_DERIVATION_EXTERN == mBalanceDerivationType; }

            protected:
                data::AccountBalance calculateAccountBalance(
                    memory::ConstBlockPtr publicKey, 
                    uint64_t maxTransactionNr, 
                    GradidoUnit amount,
                    uint32_t coinCommunityIdIndex
                ) const;

                inline data::AccountBalance calculateAccountBalance(
                  const data::TransferAmount& transferAmount,
                  uint64_t maxTransactionNr
                ) const {
                  return calculateAccountBalance(
                    transferAmount.getPublicKey(),
                    maxTransactionNr, 
                    transferAmount.getAmount(), 
                    transferAmount.getCoinCommunityIdIndex()
                  );
                }

                data::compact::AccountBalance calculateAccountBalance(
                  data::compact::PublicKeyIndex publicKeyIndex,
                  uint64_t maxTransactionNr,
                  GradidoUnit amount,
                  uint32_t coinCommunityIdIndex
                ) const;

                std::shared_ptr<const data::GradidoTransaction> mGradidoTransaction;
                data::LedgerAnchor mLedgerAnchor;
                data::Timestamp mConfirmedAt;
                std::shared_ptr<blockchain::Abstract> mBlockchain;
                grdt_balance_derivation mBalanceDerivationType;
                std::vector<data::AccountBalance> mAccountBalances;
            };
        }
    }
}

#endif // __GRADIDO_BLOCKCHAIN_INTERACTION_CONFIRM_TRANSACTION_ABSTRACT_ROLE_H