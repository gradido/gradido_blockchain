#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/CompactFilter.h"
#include "gradido_blockchain/blockchain/PublicKeySearchType.h"
#include "gradido_blockchain/blockchain/SearchDirection.h"
#include "gradido_blockchain/data/AccountBalance.h"
#include "gradido_blockchain/data/adapter/publicKey.h"
#include "gradido_blockchain/data/compact/ConfirmedGradidoTx.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/data/CrossGroupType.h"
#include "gradido_blockchain/data/TransactionTriggerEvent.h"
#include "gradido_blockchain/data/TransactionTriggerEventType.h"
#include "gradido_blockchain/interaction/confirmTransaction/RedeemDeferredTransferTransactionRole.h"

#include <loguru/loguru.hpp>
#include <magic_enum/magic_enum.hpp>

#include <memory>
#include <vector>

using namespace magic_enum;
using std::shared_ptr, std::vector;

namespace gradido {
  using blockchain::CompactFilter, blockchain::PublicKeySearchType, blockchain::SearchDirection;
  using data::AccountBalance;
  using data::adapter::toPublicKeyIndex;
  using data::compact::ConfirmedGradidoTx;
  using data::ConfirmedTransaction, data::CrossGroupType;
  using data::TransactionTriggerEvent, data::TransactionTriggerEventType;

    namespace interaction {
        namespace confirmTransaction {

            void RedeemDeferredTransferTransactionRole::runPastAddToBlockchain(
                shared_ptr<const ConfirmedTransaction> confirmedTransaction,
                shared_ptr<blockchain::Abstract> blockchain
            ) const {
                // check if this redeem whole deferred transfer amount
                auto body = confirmedTransaction->getGradidoTransaction()->getTransactionBody();
                assert(body->isRedeemDeferredTransfer());
                auto redeemDeferredTransfer = body->getRedeemDeferredTransfer();
                auto redeemAmount = redeemDeferredTransfer->getTransfer().getSender().getAmount();
                auto deferredTransferId = redeemDeferredTransfer->getDeferredTransferTransactionNr();
                auto deferredTransferEntry = blockchain->getTransactionForId(deferredTransferId);
                assert(deferredTransferEntry->getTransactionBody()->isDeferredTransfer());
                auto deferredTransfer = deferredTransferEntry->getTransactionBody()->getDeferredTransfer();
                auto transactionTriggerEventTargetDate =
                    deferredTransferEntry->getConfirmedTransaction()->getConfirmedAt().getAsTimepoint()
                    + deferredTransfer->getTimeoutDuration().getAsDuration()
                ;
                // remove timeout transaction trigger event
                blockchain->removeTransactionTriggerEvent(TransactionTriggerEvent(
                    deferredTransferId,
                    transactionTriggerEventTargetDate,
                    TransactionTriggerEventType::DEFERRED_TIMEOUT_REVERSAL
                ));
            };

            vector<AccountBalance> RedeemDeferredTransferTransactionRole::calculateAccountBalances(uint64_t maxTransactionNr) const
            {
                auto& transfer = mBody->getRedeemDeferredTransfer()->getTransfer();
                auto& transferAmount = transfer.getSender();
                auto coinCommunityIdIndex = transferAmount.getCoinCommunityIdIndex();
                auto deferredTransferEntry = mBlockchain->getTransactionForId(mBody->getRedeemDeferredTransfer()->getDeferredTransferTransactionNr());
                if (!deferredTransferEntry) {
                  LOG_F(1, "get nullptr for deferred txnr: %lu", mBody->getRedeemDeferredTransfer()->getDeferredTransferTransactionNr());
                  CompactFilter f;
                  f.publicKeySearchType = PublicKeySearchType::BalanceChangingPublicKey;
                  f.publicKeyIndex = toPublicKeyIndex(transferAmount.getPublicKey(), coinCommunityIdIndex);
                  f.searchDirection = SearchDirection::DESC;
                  f.pagination.size = 1;
                  auto txs = mBlockchain->findAll(f);
                  if (txs.size()) {
                    LOG_F(1, "correct tx should be: %lu", txs[0]->txNr);
                  }
                  throw GradidoNullPointerException("invalid deferred tx", "TransactionEntry", __FUNCTION__);
                }
                auto deferredTransferConfirmedAt = deferredTransferEntry->getConfirmedTransaction()->getConfirmedAt();
                auto deferredTransferTransaction = deferredTransferEntry->getTransactionBody()->getDeferredTransfer();
                auto& deferredTransferAmount = deferredTransferTransaction->getTransfer().getSender();
                auto decayedDeferredAmount = deferredTransferAmount.getAmount().calculateDecay(deferredTransferConfirmedAt, mConfirmedAt);
                auto change = decayedDeferredAmount - transferAmount.getAmount();
                // if recipient and original sender are identical
                if (transfer.getRecipient()->isTheSame(deferredTransferAmount.getPublicKey())) {
                    return {
                        // sender
                        AccountBalance(transferAmount.getPublicKey(), GradidoUnit::zero(), coinCommunityIdIndex),
                        // recipient and change
                        calculateAccountBalance(transfer.getRecipient(), maxTransactionNr, decayedDeferredAmount, coinCommunityIdIndex)
                    };
                }
                switch (mBody->getType()) {
                case CrossGroupType::LOCAL:
                  return {
                    // sender
                    AccountBalance(transferAmount.getPublicKey(), GradidoUnit::zero(), coinCommunityIdIndex),
                    // recipient
                    calculateAccountBalance(transfer.getRecipient(), maxTransactionNr, transferAmount.getAmount(), coinCommunityIdIndex),
                    // change back to original sender of deferred transfer
                    calculateAccountBalance(deferredTransferAmount.getPublicKey(), maxTransactionNr, change, coinCommunityIdIndex)
                  };
                case CrossGroupType::OUTBOUND:
                  return {
                    // sender
                    AccountBalance(transferAmount.getPublicKey(), GradidoUnit::zero(), coinCommunityIdIndex),
                    // change back to original sender of deferred transfer
                    calculateAccountBalance(deferredTransferAmount.getPublicKey(), maxTransactionNr, change, coinCommunityIdIndex)
                  };
                case CrossGroupType::INBOUND:
                  return {
                    // recipient
                    calculateAccountBalance(transfer.getRecipient(), maxTransactionNr, transferAmount.getAmount(), coinCommunityIdIndex),
                  };
                default:
                  throw GradidoUnhandledEnum(
                    "interaction::confirmTransaction redeem deferred transfer account balance",
                    "CrossGroupType",
                    enum_name(mBody->getType()).data()
                  );
                }
            }
        }
    }
}
