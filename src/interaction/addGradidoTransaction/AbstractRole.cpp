#include "gradido_blockchain/const.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/interaction/addGradidoTransaction/AbstractRole.h"
#include "gradido_blockchain/interaction/calculateAccountBalance/Context.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

namespace gradido {
    namespace interaction {
        namespace addGradidoTransaction {
            AbstractRole::~AbstractRole()
            {

            }

            AbstractRole::AbstractRole(std::shared_ptr<data::GradidoTransaction> gradidoTransaction, memory::ConstBlockPtr messageId, Timepoint confirmedAt)
                : mGradidoTransaction(gradidoTransaction), mMessageId(messageId), mConfirmedAt(confirmedAt)
            {
                if (!gradidoTransaction) {
                    throw GradidoNullPointerException("missing transaction", "GradidoTransactionPtr", __FUNCTION__);
                }
                if (!messageId) {
                    throw GradidoNullPointerException("missing messageId", "memory::ConstBlockPtr", __FUNCTION__);
                }
            }

            GradidoUnit AbstractRole::calculateFinalBalance(uint64_t id, const blockchain::Abstract& blockchain) const
            {
                interaction::calculateAccountBalance::Context finalBalanceCalculate(blockchain);
                return finalBalanceCalculate.run(mGradidoTransaction, mConfirmedAt, id);
            }

            std::shared_ptr<const data::ConfirmedTransaction> AbstractRole::createConfirmedTransaction(
                uint64_t id,
                std::shared_ptr<const data::ConfirmedTransaction> lastConfirmedTransaction,
                const blockchain::Abstract& blockchain
            ) const
            {
               return std::make_shared<data::ConfirmedTransaction>(
                    id,
                   mGradidoTransaction,
                   mConfirmedAt,
                   GRADIDO_CONFIRMED_TRANSACTION_V3_3_VERSION_STRING,
                   mMessageId,
                   calculateFinalBalance(id, blockchain),
                   lastConfirmedTransaction
                );

            }
        }
    }
}


