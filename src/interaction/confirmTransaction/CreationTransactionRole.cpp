#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/data/AccountBalance.h"
#include "gradido_blockchain/data/TransactionBody.h"
#include "gradido_blockchain/interaction/confirmTransaction/CreationTransactionRole.h"
#include "gradido_blockchain/memory/Block.h"

#include <memory>

using memory::Block;
using std::shared_ptr, std::make_shared;

namespace gradido {
    using namespace blockchain;
    namespace interaction {
        namespace confirmTransaction {
            std::vector<data::AccountBalance> CreationTransactionRole::calculateAccountBalances(uint64_t maxTransactionNr) const
            {
                auto& transferAmount = mBody->getTransferAmount();
                auto coinCommunityIdIndex = transferAmount.getCoinCommunityIdIndex();

                // get community root transaction for gmw and auf addresses
                auto firstTransactionEntry = mBlockchain->findOne(Filter::FIRST_TRANSACTION);
                assert(firstTransactionEntry->getTransactionBody()->isCommunityRoot());
                auto communityRoot = firstTransactionEntry->getTransactionBody()->getCommunityRoot().value();
                auto gmwPubkey = g_appContext->getPublicKey(communityRoot.gmwPublicKeyIndex).value();
                auto gmwPubkeyPtr = make_shared<const Block>(gmwPubkey);
                auto aufPubkey = g_appContext->getPublicKey(communityRoot.aufPublicKeyIndex).value();
                auto aufPubkeyPtr = make_shared<const Block>(aufPubkey);
                
                return {
                    // user which get creation
                    calculateAccountBalance(transferAmount.getPublicKey(), maxTransactionNr, transferAmount.getAmount(), coinCommunityIdIndex),
                    // gmw
                    calculateAccountBalance(gmwPubkeyPtr, maxTransactionNr, transferAmount.getAmount(), coinCommunityIdIndex),
                    // auf
                    calculateAccountBalance(aufPubkeyPtr, maxTransactionNr, transferAmount.getAmount(), coinCommunityIdIndex),
                };
            }
        }
    }
}