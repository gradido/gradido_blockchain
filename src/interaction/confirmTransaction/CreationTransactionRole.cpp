#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/data/adapter/publicKey.h"
#include "gradido_blockchain/data/AccountBalance.h"
#include "gradido_blockchain/data/compact/AccountBalance.h"
#include "gradido_blockchain/data/compact/CommunityRootTx.h"
#include "gradido_blockchain/data/compact/PublicKeyIndex.h"
#include "gradido_blockchain/data/TransactionBody.h"
#include "gradido_blockchain/interaction/confirmTransaction/CreationTransactionRole.h"
#include "gradido_blockchain/memory/Block.h"

#include <memory>

using memory::Block;
using std::shared_ptr, std::make_shared;
using std::vector;

namespace gradido {
    using namespace blockchain;
    using data::AccountBalance;
    using data::adapter::toConstBlockPtr;
    using data::compact::CommunityRootTx, data::compact::PublicKeyIndex;
    namespace interaction {
        namespace confirmTransaction {

            vector<AccountBalance> CreationTransactionRole::calculateAccountBalances(uint64_t maxTransactionNr) const
            {
                auto& transferAmount = mBody->getTransferAmount();

                // get community root transaction for gmw and auf addresses
                auto firstTransactionEntry = mBlockchain->findOne(Filter::FIRST_TRANSACTION);
                assert(firstTransactionEntry->getTransactionBody()->isCommunityRoot());
                auto communityRoot = firstTransactionEntry->getTransactionBody()->getCommunityRoot();
                if (!communityRoot) {
                  throw GradidoNodeInvalidDataException("invalid first transaction, expect community root tx");
                }

                return {
                  // user which get creation
                    calculateAccountBalance(transferAmount, maxTransactionNr),
                    // gmw
                    accountBalanceFromPublicKeyIndex(maxTransactionNr, communityRoot->gmwPublicKeyIndex, *communityRoot),
                    // auf
                    accountBalanceFromPublicKeyIndex(maxTransactionNr, communityRoot->aufPublicKeyIndex, *communityRoot)
                };
            }

            AccountBalance CreationTransactionRole::accountBalanceFromPublicKeyIndex(
              uint64_t maxTransactionNr,
              uint32_t publicKeyIndex, 
              CommunityRootTx& startTransaction
            ) const 
            {
              auto& transferAmount = mBody->getTransferAmount();
              auto coinCommunityIdIndex = transferAmount.getCoinCommunityIdIndex();
              auto blockchainCommunityIdIndex = mBlockchain->getCommunityIdIndex();
              PublicKeyIndex pubkeyIndexObj = {
                .communityIdIndex = blockchainCommunityIdIndex,
                .publicKeyIndex = publicKeyIndex
              };
              auto accountBalance = calculateAccountBalance(
                pubkeyIndexObj,
                maxTransactionNr,
                transferAmount.getAmount(),
                coinCommunityIdIndex
              );
              return { 
                toConstBlockPtr(pubkeyIndexObj), 
                GradidoUnit::fromGradidoCent(accountBalance.balanceGddCent), 
                coinCommunityIdIndex 
              };
            }
        }
    }
}