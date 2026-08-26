#include "gradido_blockchain_core/data/wire/ledger_anchor.h"
#include "arnm/memory_block.h"
#include "gradido_blockchain/data/adapter/hiero.h"
#include "gradido_blockchain/data/adapter/ledgerAnchor.h"
#include "gradido_blockchain/data/adapter/timestamp.h"
#include "gradido_blockchain/data/LedgerAnchor.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/memory/Block.h"

#include <string>

using memory::Block;
using std::to_string;

namespace gradido::data {
    namespace adapter {
      LedgerAnchor fromGrdw(const grdw_ledger_anchor& ledgerAnchor)
      {
        if (GRDT_LEDGER_ANCHOR_UNSPECIFIED == ledgerAnchor.type) {
          return {};
        }
        else if (GRDT_LEDGER_ANCHOR_HIERO_TRANSACTION_ID == ledgerAnchor.type) {
          return LedgerAnchor(fromGrdw(ledgerAnchor.hiero_transaction_id));
        }
        
        return LedgerAnchor(ledgerAnchor.id, ledgerAnchor.type);        
      }
      grdw_ledger_anchor toGrdw(arnm* alloc, const LedgerAnchor& ledgerAnchor)
      {        
        auto type = ledgerAnchor.getType();
        if (GRDT_LEDGER_ANCHOR_UNSPECIFIED == type) {
          return {
              .type = GRDT_LEDGER_ANCHOR_UNSPECIFIED
          };
        }
        else if (ledgerAnchor.isHieroTransactionId()) {
          const auto& hieroTxId = ledgerAnchor.getHieroTransactionIdCompact();
          return {
            .type = GRDT_LEDGER_ANCHOR_HIERO_TRANSACTION_ID,
            .hiero_transaction_id = {
              .transactionValidStart = {
                .seconds = hieroTxId.seconds, 
                .nanos = hieroTxId.nanos 
              },
              .accountID = {
                .shardNum = 0,
                .realmNum = 0,
                .accountNum = hieroTxId.accountNum 
              }
            }
          };
        }        
        if (GRDT_LEDGER_ANCHOR_NODE_TRIGGER_TRANSACTION_ID == type) {
          return {
            .type = type,
            .id = ledgerAnchor.getNodeTriggeredTransactionId()
          };
        }
        return {
          .type = type,
          .id = ledgerAnchor.getLegacyGradidoDbId()
        };
      }
    }
}
