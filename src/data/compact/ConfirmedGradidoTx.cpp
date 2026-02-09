#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/data/adapter/types.h"
#include "gradido_blockchain/data/compact/ConfirmedGradidoTx.h"
#include "gradido_blockchain/data/compact/ConfirmedGradidoTxCold.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

namespace gradido::data::compact {

  ConfirmedGradidoTx::ConfirmedGradidoTx()
    : txNr(0), confirmedAtSeconds(0), confirmedAtNanos(0), txCommunityIdIndex(0),
    crossGroupType(CrossGroupType::LOCAL), transactionType(TransactionType::NONE), balanceDerivationType(BalanceDerivationType::UNSPECIFIED),
    accountBalanceCount(0),
    coldData(new ConfirmedGradidoTxCold)
  {
  }

  ConfirmedGradidoTx::~ConfirmedGradidoTx()
  {
    delete coldData;
    coldData = nullptr;
  }

  ConfirmedGradidoTx ConfirmedGradidoTx::fromGrdw(
    const grdw_confirmed_transaction* tx,
    const grdw_transaction_body* body,
    uint32_t blockchainCommunityIdIndex
  ) {
    ConfirmedGradidoTx confirmedTx;
    confirmedTx.txNr = tx->id;
    confirmedTx.confirmedAtSeconds = tx->confirmed_at.seconds;
    confirmedTx.confirmedAtNanos = tx->confirmed_at.nanos;
    confirmedTx.txCommunityIdIndex = blockchainCommunityIdIndex;
    confirmedTx.crossGroupType = adapter::fromGrdw(body->type);
    confirmedTx.transactionType = adapter::fromGrdw(body->transaction_type);
    confirmedTx.balanceDerivationType = adapter::fromGrdw(tx->balance_derivation);
    if (tx->account_balances_count > 3) {
      throw GradidoNotImplementedException("more than 3 account balances, currently not supported from compact::ConfirmedGradidoTx");
    }
    confirmedTx.accountBalanceCount = tx->account_balances_count;
    for (int i = 0; i < tx->account_balances_count; i++) 
    {
      confirmedTx.accountBalances[i].balanceGddCent = tx->account_balances[i].balance;
      if (!tx->account_balances[i].community_id) {
        confirmedTx.accountBalances[i].coinCommunityIdIndex = blockchainCommunityIdIndex;
      }
      else {
        confirmedTx.accountBalances[i].coinCommunityIdIndex = g_appContext->getOrAddCommunityIdIndex(tx->account_balances[i].community_id);
      }
      // confirmedTx.accountBalances[i].publicKeyIndex = g_appContext->
       // tx->account_balances[i]
    }
    return confirmedTx;
  }
}