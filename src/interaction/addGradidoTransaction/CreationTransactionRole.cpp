#include "gradido_blockchain/interaction/addGradidoTransaction/CreationTransactionRole.h"
#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/data/AccountBalance.h"
#include "gradido_blockchain/data/TransactionBody.h"

namespace gradido {
    using namespace blockchain;
    namespace interaction {
        namespace addGradidoTransaction {
            std::vector<data::AccountBalance> CreationTransactionRole::calculateAccountBalances(uint64_t maxTransactionNr) const
            {
                auto transfer = mBody->getTransfer();
                auto& transferAmount = transfer->getSender();

                // get community root transaction for gmw and auf addresses
                auto firstTransactionEntry = mBlockchain->findOne(Filter::FIRST_TRANSACTION);
                assert(firstTransactionEntry->getTransactionBody()->isCommunityRoot());
                auto communityRoot = firstTransactionEntry->getTransactionBody()->getCommunityRoot();

                return {
                    // user which get creation
                    calculateAccountBalance(transferAmount.getPublicKey(), maxTransactionNr, transferAmount.getAmount()),
                    // gmw
                    calculateAccountBalance(communityRoot->getGmwPubkey(), maxTransactionNr, transferAmount.getAmount()),
                    // auf
                    calculateAccountBalance(communityRoot->getAufPubkey(), maxTransactionNr, transferAmount.getAmount()),
                };
            }
        }
    }
}