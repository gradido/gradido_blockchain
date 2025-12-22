#include "gradido_blockchain/const.h"
#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/FilterBuilder.h"
#include "gradido_blockchain/data/BalanceDerivationType.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/data/Timestamp.h"
#include "gradido_blockchain/interaction/confirmTransaction/AbstractRole.h"
#include "gradido_blockchain/interaction/advancedBlockchainFilter/Context.h"
#include "gradido_blockchain/interaction/calculateAccountBalance/Context.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

#include "magic_enum/magic_enum.hpp"

using namespace std;
using namespace magic_enum;

namespace gradido {
    using namespace blockchain;
    using namespace data;

    namespace interaction {
        namespace confirmTransaction {
            AbstractRole::~AbstractRole()
            {

            }

            AbstractRole::AbstractRole(
                std::shared_ptr<const data::GradidoTransaction> gradidoTransaction,
                const data::LedgerAnchor& ledgerAnchor,
                Timestamp confirmedAt,
                std::shared_ptr<blockchain::Abstract> blockchain
            ): mGradidoTransaction(gradidoTransaction), mLedgerAnchor(ledgerAnchor), mConfirmedAt(confirmedAt), mBlockchain(blockchain), mBalanceDerivationType(BalanceDerivationType::NODE)
            {
                if (!gradidoTransaction) {
                    throw GradidoNullPointerException("missing transaction", "GradidoTransactionPtr", __FUNCTION__);
                }
            }

            std::shared_ptr<const data::ConfirmedTransaction> AbstractRole::createConfirmedTransaction(
                uint64_t id,
                std::shared_ptr<const ConfirmedTransaction> lastConfirmedTransaction
            ) {
                std::vector<AccountBalance> accountBalances;
                if (BalanceDerivationType::NODE == mBalanceDerivationType) {
                    accountBalances = calculateAccountBalances(id - 1);
                }
                else {
                    accountBalances = mAccountBalances;
                }
                return make_shared<data::ConfirmedTransaction>(
                   id,
                   //std::make_unique<data::GradidoTransaction>(*mGradidoTransaction),
                   std::make_shared<data::GradidoTransaction>(*mGradidoTransaction),
                   //mGradidoTransaction, // don't work as native node module. TODO: find underlying issue
                   mConfirmedAt,
                   GRADIDO_CONFIRMED_TRANSACTION_VERSION_STRING,
                   mLedgerAnchor,
                   accountBalances,
                   mBalanceDerivationType,
                   lastConfirmedTransaction
                );
            }

            void AbstractRole::setAccountBalances(std::vector<data::AccountBalance> accountBalances)
            {
                mAccountBalances = accountBalances;
                mBalanceDerivationType = BalanceDerivationType::EXTERN;
            }

            AccountBalance AbstractRole::calculateAccountBalance(
                memory::ConstBlockPtr publicKey,
                uint64_t maxTransactionNr,
                GradidoUnit amount,
                const std::string& communityId
            ) const 
            {
                FilterBuilder builder;
                GradidoUnit previousDecayedAccountBalance;
                auto f = Filter::lastBalanceFor(publicKey);
                f.coinCommunityId = communityId;
                const auto& lastBalanceChangingTransaction = mBlockchain->findOne(f);
                if (lastBalanceChangingTransaction) {
                    previousDecayedAccountBalance = lastBalanceChangingTransaction->getConfirmedTransaction()->getDecayedAccountBalance(publicKey, communityId, mConfirmedAt);
                }
                GradidoUnit newBalance = previousDecayedAccountBalance + amount;
                if (newBalance < GradidoUnit::zero()) {
                    if (newBalance + GradidoUnit::fromGradidoCent(100) < GradidoUnit::zero()) {
                        throw InsufficientBalanceException(
                            "not enough Gradido Balance for operation",
                            amount,
                            previousDecayedAccountBalance
                        );
                    } else {
                        newBalance = GradidoUnit::zero();
                    }
                }
                return AccountBalance(publicKey, newBalance, communityId);
            }
        }
    }
}


