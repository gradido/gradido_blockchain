#include "gradido_blockchain_core/data/wire/confirmed_transaction.h"
#include "gradido_blockchain_core/data/wire/transaction_body.h"
#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/data/compact/ConfirmedGradidoTxCold.h"
#include "gradido_blockchain/data/adapter/ledgerAnchor.h"
#include "gradido_blockchain/data/adapter/types.h"
#include "gradido_blockchain/memory/Block.h"

#include <utility>

using memory::Block;
using std::make_pair, std::unique_ptr, std::make_unique;

namespace gradido::data {
  namespace compact {
    ConfirmedGradidoTxCold::ConfirmedGradidoTxCold()
      : createdAtSeconds(0), createdAtNanos(0), pairingTxCommunityIdIndex(0), bodyBytes(0)
    {

    }

    unique_ptr<ConfirmedGradidoTxCold> ConfirmedGradidoTxCold::fromGrdw(const grdw_confirmed_transaction* tx)
    {
      if (!tx) throw GradidoNullPointerException("empty tx", "grdw_confirmed_transaction", __FUNCTION__);
      
      auto txCold = fromGrdw(&tx->transaction);
      txCold->runningHash = tx->running_hash;
      txCold->ledgerAnchor = adapter::fromGrdw(tx->ledger_anchor);

      return txCold;
    }

    unique_ptr<ConfirmedGradidoTxCold> ConfirmedGradidoTxCold::fromGrdw(
      const grdw_confirmed_transaction* tx,
      const grdw_transaction_body* body,
      AppContext& appContext
    ) {
      auto txCold = fromGrdw(tx);
      txCold->fillFromGrdwTransactionBody(body, appContext);
      return txCold;
    }

    unique_ptr<ConfirmedGradidoTxCold> ConfirmedGradidoTxCold::fromGrdw(const grdw_gradido_transaction* tx)
    {
      if (!tx) throw GradidoNullPointerException("empty tx", "grdw_confirmed_transaction", __FUNCTION__);

      auto txCold = make_unique<ConfirmedGradidoTxCold>();

      // pairing tx
      if (tx->pairing_ledger_anchor.type != GRDW_LEDGER_ANCHOR_TYPE_UNSPECIFIED) {
        txCold->pairingLedgerAnchor = adapter::fromGrdw(tx->pairing_ledger_anchor);
      }
      // signatures
      if (tx->sig_map_count) {
        txCold->signatureMap.reserve(tx->sig_map_count);
        for (int i = 0; i < tx->sig_map_count; i++) {
          txCold->signatureMap.emplace_back(make_pair(
            PublicKey(tx->sig_map[i].public_key),
            Signature(tx->sig_map[i].signature)
          ));
        }
      }
      txCold->bodyBytes = Block(tx->body_bytes.size, tx->body_bytes.data);
      return txCold;
    }

    unique_ptr<ConfirmedGradidoTxCold> ConfirmedGradidoTxCold::fromGrdw(
      const grdw_gradido_transaction* tx,
      const grdw_transaction_body* body,
      AppContext& appContext
    ) {
      auto txCold = fromGrdw(tx);
      txCold->fillFromGrdwTransactionBody(body, appContext);
      return txCold;
    }

    void ConfirmedGradidoTxCold::fillFromGrdwTransactionBody(const grdw_transaction_body* body, AppContext& appContext)
    {
      if (!body) throw GradidoNullPointerException("empty body pointer", "grdw_transaction_body", __FUNCTION__);

      // created at
      createdAtSeconds = body->created_at.seconds;
      createdAtNanos = body->created_at.nanos;

      // pairing tx
      if (!pairingLedgerAnchor.empty() && body->other_community_uuid) {
        pairingTxCommunityIdIndex = appContext.getOrAddCommunityIdIndex(Uuid(body->other_community_uuid));
      }
      if (body->memos_count > 2) {
        throw GradidoNotImplementedException("memo count is more than expected");
      }
      // memos
      for (int i = 0; i < body->memos_count; i++) {
        encryptedMemos[i] = EncryptedMemo(adapter::fromGrdw(body->memos[i].type), Block(body->memos[i].memo.size, body->memos[i].memo.data));
      }
    }
  }
}