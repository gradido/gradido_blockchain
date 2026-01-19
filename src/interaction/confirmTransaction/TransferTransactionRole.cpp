#include "gradido_blockchain/interaction/confirmTransaction/TransferTransactionRole.h"
#include "gradido_blockchain/data/AccountBalance.h"
#include "gradido_blockchain/data/TransactionBody.h"

namespace gradido {
    using namespace blockchain;
    using namespace data;

    namespace interaction {
        namespace confirmTransaction {
            std::vector<data::AccountBalance> TransferTransactionRole::calculateAccountBalances(uint64_t maxTransactionNr) const
            {
                auto transfer = mBody->getTransfer();
                auto& transferAmount = transfer->getSender();
                auto coinCommunityIdIndex = transferAmount.getCoinCommunityIdIndex();

                return {
                    // sender
                    calculateAccountBalance(transferAmount.getPublicKey(), maxTransactionNr, transferAmount.getAmount().negated(), coinCommunityIdIndex),
                    // recipient
                    calculateAccountBalance(transfer->getRecipient(), maxTransactionNr, transferAmount.getAmount(), coinCommunityIdIndex)
                };

            }
        }
    }
}
