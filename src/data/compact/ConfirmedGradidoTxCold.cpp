#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/data/compact/ConfirmedGradidoTxCold.h"
#include "gradido_blockchain/data/adapter/ledgerAnchor.h"
#include "gradido_blockchain/data/adapter/types.h"
#include "gradido_blockchain/memory/Block.h"

#include <utility>

using memory::Block;
using std::make_pair;

namespace gradido::data {
  namespace compact {
    ConfirmedGradidoTxCold::ConfirmedGradidoTxCold()
      : createdAtSeconds(0), createdAtNanos(0), pairingTxCommunityIdIndex(0), bodyBytes(0)
    {

    }

    ConfirmedGradidoTxCold* ConfirmedGradidoTxCold::fromGrdw(
      const grdw_confirmed_transaction* tx,
      const grdw_transaction_body* body,
      uint32_t blockchainCommunityIdIndex
    ) {
      if (!tx) throw GradidoNullPointerException("empty tx", "grdw_confirmed_transaction", __FUNCTION__);
      if (!body) throw GradidoNullPointerException("empty body pointer", "grdw_transaction_body", __FUNCTION__);

      auto txCold = new ConfirmedGradidoTxCold();

      // created at
      txCold->createdAtSeconds = body->created_at.seconds;
      txCold->createdAtNanos = body->created_at.nanos;

      // pairing tx
      if (tx->transaction.pairing_ledger_anchor.type != GRDW_LEDGER_ANCHOR_TYPE_UNSPECIFIED) {
        txCold->pairingLedgerAnchor = adapter::fromGrdw(tx->transaction.pairing_ledger_anchor);
        txCold->pairingTxCommunityIdIndex = g_appContext->getOrAddCommunityIdIndex(body->other_group);
        if (!txCold->pairingTxCommunityIdIndex) {
          throw GradidoNodeInvalidDataException("invalid other community id for pairing ledger anchor");
        }
      }
      if (body->memos_count > 2) {
        throw GradidoNotImplementedException("memo count is more than expected");
      }
      // memos
      for (int i = 0; i < body->memos_count; i++) {
        //encryptedMemos[i] = EncryptedMemo(adapter::fromGrdw(body->memos[i].type), Block(body->memos[i].memo_size, body->memos[i].memo));
      }
      // signatures
      if (tx->transaction.sig_map_count) {
        txCold->signatureMap.reserve(tx->transaction.sig_map_count);
        for (int i = 0; i < tx->transaction.sig_map_count; i++) {
          txCold->signatureMap.emplace_back(make_pair(
            PublicKey(tx->transaction.sig_map[i].public_key), 
            Signature(tx->transaction.sig_map[i].signature)
          ));
        }
      }
      // running hash
      txCold->runningHash = tx->running_hash;

      txCold->ledgerAnchor = adapter::fromGrdw(tx->ledger_anchor);

      txCold->bodyBytes = Block(tx->transaction.body_bytes_size, tx->transaction.body_bytes);
      return txCold;
    }
  }
}