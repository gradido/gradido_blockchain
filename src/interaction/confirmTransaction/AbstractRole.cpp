#include "gradido_blockchain/const.h"
#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/FilterBuilder.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
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
                memory::ConstBlockPtr messageId,
                Timepoint confirmedAt,
                std::shared_ptr<blockchain::Abstract> blockchain
            ): mGradidoTransaction(gradidoTransaction), mMessageId(messageId), mConfirmedAt(confirmedAt), mBlockchain(blockchain)
            {
                if (!gradidoTransaction) {
                    throw GradidoNullPointerException("missing transaction", "GradidoTransactionPtr", __FUNCTION__);
                }
            }

            std::shared_ptr<const data::ConfirmedTransaction> AbstractRole::createConfirmedTransaction(
                uint64_t id,
                std::shared_ptr<const ConfirmedTransaction> lastConfirmedTransaction
            ) const {
               return make_shared<data::ConfirmedTransaction>(
                   id,
                   mGradidoTransaction,
                   mConfirmedAt,
                   GRADIDO_CONFIRMED_TRANSACTION_VERSION_STRING,
                   mMessageId,
                   calculateAccountBalances(id - 1),
                   lastConfirmedTransaction
                );

            }

            AccountBalance AbstractRole::calculateAccountBalance(memory::ConstBlockPtr publicKey, uint64_t maxTransactionNr, GradidoUnit amount) const
            {
                FilterBuilder builder;
                GradidoUnit previousDecayedAccountBalance;
                mBlockchain->findOne(builder
                    .setInvolvedPublicKey(publicKey)
                    .setSearchDirection(SearchDirection::DESC)
                    .setMaxTransactionNr(maxTransactionNr)
                    .setFilterFunction([publicKey, &previousDecayedAccountBalance, this](const TransactionEntry& entry) -> FilterResult
                        {
                            auto confirmedTransction = entry.getConfirmedTransaction();
                            if (confirmedTransction->hasAccountBalance(*publicKey)) {
                                previousDecayedAccountBalance = confirmedTransction->getDecayedAccountBalance(publicKey, mConfirmedAt);
                                return FilterResult::STOP;
                            }
                            return FilterResult::DISMISS;
                        })
                    .build()
                );
                return AccountBalance(publicKey, previousDecayedAccountBalance + amount);
            }
        }
    }
}


