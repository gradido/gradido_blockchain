#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/data/adapter/publicKey.h"
#include "gradido_blockchain/data/AccountBalance.h"
#include "gradido_blockchain/data/TransactionBody.h"
#include "gradido_blockchain/interaction/confirmTransaction/CreationTransactionRole.h"
#include "gradido_blockchain/memory/Block.h"

#include <memory>

using memory::Block;
using std::shared_ptr, std::make_shared;

namespace gradido {
    using namespace blockchain;
    using data::adapter::toConstBlockPtr;
    namespace interaction {
        namespace confirmTransaction {
            std::vector<data::AccountBalance> CreationTransactionRole::calculateAccountBalances(uint64_t maxTransactionNr) const
            {
                auto& transferAmount = mBody->getTransferAmount();
                auto coinCommunityIdIndex = transferAmount.getCoinCommunityIdIndex();

                // get community root transaction for gmw and auf addresses
                auto firstTransactionEntry = mBlockchain->findOne(Filter::FIRST_TRANSACTION);
                assert(firstTransactionEntry->getTransactionBody()->isCommunityRoot());
                const auto& communityRoot = firstTransactionEntry->getTransactionBody()->getCommunityRoot().value();
                auto gmwPublicKey = toConstBlockPtr({ firstTransactionEntry->getBlockchainCommunityIdIndex(), communityRoot.gmwPublicKeyIndex });
                auto aufPublicKey = toConstBlockPtr({ firstTransactionEntry->getBlockchainCommunityIdIndex(), communityRoot.aufPublicKeyIndex });

                return {
                    // user which get creation
                    calculateAccountBalance(transferAmount.getPublicKey(), maxTransactionNr, transferAmount.getAmount(), coinCommunityIdIndex),
                    // gmw
                    calculateAccountBalance(gmwPublicKey, maxTransactionNr, transferAmount.getAmount(), coinCommunityIdIndex),
                    // auf
                    calculateAccountBalance(aufPublicKey, maxTransactionNr, transferAmount.getAmount(), coinCommunityIdIndex),
                };
            }
        }
    }
}