#include "gradido_blockchain/interaction/confirmTransaction/CreationTransactionRole.h"
#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/data/AccountBalance.h"
#include "gradido_blockchain/data/TransactionBody.h"

namespace gradido {
    using namespace blockchain;
    namespace interaction {
        namespace confirmTransaction {
            std::vector<data::AccountBalance> CreationTransactionRole::calculateAccountBalances(uint64_t maxTransactionNr) const
            {
                auto& transferAmount = mBody->getTransferAmount();
                auto& communityId = transferAmount.getCommunityId();

                // get community root transaction for gmw and auf addresses
                auto firstTransactionEntry = mBlockchain->findOne(Filter::FIRST_TRANSACTION);
                assert(firstTransactionEntry->getTransactionBody()->isCommunityRoot());
                auto communityRoot = firstTransactionEntry->getTransactionBody()->getCommunityRoot();

                return {
                    // user which get creation
                    calculateAccountBalance(transferAmount.getPublicKey(), maxTransactionNr, transferAmount.getAmount(), communityId),
                    // gmw
                    calculateAccountBalance(communityRoot->getGmwPubkey(), maxTransactionNr, transferAmount.getAmount(), communityId),
                    // auf
                    calculateAccountBalance(communityRoot->getAufPubkey(), maxTransactionNr, transferAmount.getAmount(), communityId),
                };
            }
        }
    }
}