#include "gradido_blockchain_core/data/wire/ledger_anchor.h"
#include "gradido_blockchain_core/memory.h"
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
        switch (ledgerAnchor.type) {
        case GRDW_LEDGER_ANCHOR_TYPE_UNSPECIFIED: 
          return {};
        case GRDW_LEDGER_ANCHOR_TYPE_HIERO_TRANSACTION_ID:
          return LedgerAnchor(fromGrdw(ledgerAnchor.hiero_transaction_id));
        case GRDW_LEDGER_ANCHOR_TYPE_LEGACY_GRADIDO_DB_TRANSACTION_ID:
          return LedgerAnchor(ledgerAnchor.id, LedgerAnchor::Type::LEGACY_GRADIDO_DB_TRANSACTION_ID);
        case GRDW_LEDGER_ANCHOR_TYPE_NODE_TRIGGER_TRANSACTION_ID:
          return LedgerAnchor(ledgerAnchor.id, LedgerAnchor::Type::NODE_TRIGGER_TRANSACTION_ID);
        case GRDW_LEDGER_ANCHOR_TYPE_LEGACY_GRADIDO_DB_COMMUNITY_ID:
          return LedgerAnchor(ledgerAnchor.id, LedgerAnchor::Type::LEGACY_GRADIDO_DB_COMMUNITY_ID);
        case GRDW_LEDGER_ANCHOR_TYPE_LEGACY_GRADIDO_DB_USER_ID:
          return LedgerAnchor(ledgerAnchor.id, LedgerAnchor::Type::LEGACY_GRADIDO_DB_USER_ID);
        case GRDW_LEDGER_ANCHOR_TYPE_LEGACY_GRADIDO_DB_CONTRIBUTION_ID:
          return LedgerAnchor(ledgerAnchor.id, LedgerAnchor::Type::LEGACY_GRADIDO_DB_CONTRIBUTION_ID);
        case GRDW_LEDGER_ANCHOR_TYPE_LEGACY_GRADIDO_DB_TRANSACTION_LINK_ID:
          return LedgerAnchor(ledgerAnchor.id, LedgerAnchor::Type::LEGACY_GRADIDO_DB_TRANSACTION_LINK_ID);
        default:
          throw GradidoUnhandledEnum("error converting ledger anchor to grdw (C-Interface)", "LedgerAnchor", to_string(static_cast<uint8_t>(ledgerAnchor.type)).c_str());
        }
        
      }
      grdw_ledger_anchor toGrdw(grd_memory* alloc, const LedgerAnchor& ledgerAnchor)
      {        
        if (ledgerAnchor.isHieroTransactionId()) {
          const auto& hieroTxId = ledgerAnchor.getHieroTransactionIdCompact();
          return {
            .type = GRDW_LEDGER_ANCHOR_TYPE_HIERO_TRANSACTION_ID,
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
        switch(ledgerAnchor.getType()) {
          case LedgerAnchor::Type::IOTA_MESSAGE_ID:
            throw GradidoUnhandledEnum("error converting ledger anchor to grdw (C-Interface)", "LedgerAnchor", to_string(static_cast<uint8_t>(ledgerAnchor.getType())).c_str());
          case LedgerAnchor::Type::LEGACY_GRADIDO_DB_TRANSACTION_ID:
            return {
              .type = GRDW_LEDGER_ANCHOR_TYPE_LEGACY_GRADIDO_DB_TRANSACTION_ID,
              .id = ledgerAnchor.getLegacyTransactionId()
            };
          case LedgerAnchor::Type::NODE_TRIGGER_TRANSACTION_ID:
            return {
              .type = GRDW_LEDGER_ANCHOR_TYPE_NODE_TRIGGER_TRANSACTION_ID,
              .id = ledgerAnchor.getNodeTriggeredTransactionId()
            };
          case LedgerAnchor::Type::LEGACY_GRADIDO_DB_COMMUNITY_ID:
            return {
              .type = GRDW_LEDGER_ANCHOR_TYPE_LEGACY_GRADIDO_DB_COMMUNITY_ID,
              .id = ledgerAnchor.getLegacyTransactionId()
            };
          case LedgerAnchor::Type::LEGACY_GRADIDO_DB_USER_ID:
            return {
              .type = GRDW_LEDGER_ANCHOR_TYPE_LEGACY_GRADIDO_DB_USER_ID,
              .id = ledgerAnchor.getLegacyTransactionId()
            };
          case LedgerAnchor::Type::LEGACY_GRADIDO_DB_CONTRIBUTION_ID:
            return {
              .type = GRDW_LEDGER_ANCHOR_TYPE_LEGACY_GRADIDO_DB_CONTRIBUTION_ID,
              .id = ledgerAnchor.getLegacyTransactionId()
            };
          case LedgerAnchor::Type::LEGACY_GRADIDO_DB_TRANSACTION_LINK_ID:
            return {
              .type = GRDW_LEDGER_ANCHOR_TYPE_LEGACY_GRADIDO_DB_TRANSACTION_LINK_ID,
              .id = ledgerAnchor.getLegacyTransactionId()
            };
          case LedgerAnchor::Type::UNSPECIFIED:
            return {
              .type = GRDW_LEDGER_ANCHOR_TYPE_UNSPECIFIED
            };
          default:
            throw GradidoUnhandledEnum("error converting ledger anchor to grdw (C-Interface)", "LedgerAnchor", to_string(static_cast<uint8_t>(ledgerAnchor.getType())).c_str());
        }
      }
    }
}
