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
        case GRDW_LEDGER_ANCHOR_TYPE_IOTA_MESSAGE_ID: 
          return LedgerAnchor(Block(32, ledgerAnchor.anchor_id.iota_message_id));
        case GRDW_LEDGER_ANCHOR_TYPE_HIERO_TRANSACTION_ID:
          return LedgerAnchor(fromGrdw(*ledgerAnchor.anchor_id.hiero_transaction_id));
        case GRDW_LEDGER_ANCHOR_TYPE_LEGACY_GRADIDO_DB_TRANSACTION_ID:
          return LedgerAnchor(ledgerAnchor.anchor_id.legacy_transaction_id, LedgerAnchor::Type::LEGACY_GRADIDO_DB_TRANSACTION_ID);
        case GRDW_LEDGER_ANCHOR_TYPE_NODE_TRIGGER_TRANSACTION_ID:
          return LedgerAnchor(ledgerAnchor.anchor_id.node_trigger_transaction_id, LedgerAnchor::Type::NODE_TRIGGER_TRANSACTION_ID);
        case GRDW_LEDGER_ANCHOR_TYPE_LEGACY_GRADIDO_DB_COMMUNITY_ID:
          return LedgerAnchor(ledgerAnchor.anchor_id.legacy_community_id, LedgerAnchor::Type::LEGACY_GRADIDO_DB_COMMUNITY_ID);
        case GRDW_LEDGER_ANCHOR_TYPE_LEGACY_GRADIDO_DB_USER_ID:
          return LedgerAnchor(ledgerAnchor.anchor_id.legacy_user_id, LedgerAnchor::Type::LEGACY_GRADIDO_DB_USER_ID);
        case GRDW_LEDGER_ANCHOR_TYPE_LEGACY_GRADIDO_DB_CONTRIBUTION_ID:
          return LedgerAnchor(ledgerAnchor.anchor_id.legacy_contribution_id, LedgerAnchor::Type::LEGACY_GRADIDO_DB_CONTRIBUTION_ID);
        case GRDW_LEDGER_ANCHOR_TYPE_LEGACY_GRADIDO_DB_TRANSACTION_LINK_ID:
          return LedgerAnchor(ledgerAnchor.anchor_id.legacy_transaction_link_id, LedgerAnchor::Type::LEGACY_GRADIDO_DB_TRANSACTION_LINK_ID);
        default:
          throw GradidoUnhandledEnum("error converting ledger anchor to grdw (C-Interface)", "LedgerAnchor", to_string(static_cast<uint8_t>(ledgerAnchor.type)).c_str());
        }
        
      }
      grdw_ledger_anchor toGrdw(const LedgerAnchor& ledgerAnchor)
      {        
        if (ledgerAnchor.isHieroTransactionId()) {
          const auto& hieroTxId = ledgerAnchor.getHieroTransactionIdCompact();
          grdw_timestamp transactionValidStart = { .seconds = hieroTxId.seconds, .nanos = hieroTxId.nanos };
          grdw_hiero_account_id accountID = { .shardNum = 0, .realmNum = 0, .accountNum = hieroTxId.accountNum };
          return {
            .type = GRDW_LEDGER_ANCHOR_TYPE_HIERO_TRANSACTION_ID,
            .anchor_id = { .hiero_transaction_id = grdw_hiero_transaction_id_new(&transactionValidStart, &accountID) }
          };
        }        
        switch(ledgerAnchor.getType()) {
          case LedgerAnchor::Type::IOTA_MESSAGE_ID:
            assert(ledgerAnchor.getIotaMessageId().size() == 32);
            return {
              .type = GRDW_LEDGER_ANCHOR_TYPE_IOTA_MESSAGE_ID,
              .anchor_id = { .iota_message_id = grdu_reserve_copy(ledgerAnchor.getIotaMessageId().data(), 32) }
            };
          case LedgerAnchor::Type::LEGACY_GRADIDO_DB_TRANSACTION_ID:
            return {
              .type = GRDW_LEDGER_ANCHOR_TYPE_LEGACY_GRADIDO_DB_TRANSACTION_ID,
              .anchor_id = { .legacy_transaction_id = ledgerAnchor.getLegacyTransactionId() }
            };
          case LedgerAnchor::Type::NODE_TRIGGER_TRANSACTION_ID:
            return {
              .type = GRDW_LEDGER_ANCHOR_TYPE_NODE_TRIGGER_TRANSACTION_ID,
              .anchor_id = { .node_trigger_transaction_id = ledgerAnchor.getNodeTriggeredTransactionId() }
            };
          case LedgerAnchor::Type::LEGACY_GRADIDO_DB_COMMUNITY_ID:
            return {
              .type = GRDW_LEDGER_ANCHOR_TYPE_LEGACY_GRADIDO_DB_COMMUNITY_ID,
              .anchor_id = { .legacy_community_id = ledgerAnchor.getLegacyTransactionId() }
            };
          case LedgerAnchor::Type::LEGACY_GRADIDO_DB_USER_ID:
            return {
              .type = GRDW_LEDGER_ANCHOR_TYPE_LEGACY_GRADIDO_DB_USER_ID,
              .anchor_id = { .legacy_user_id = ledgerAnchor.getLegacyTransactionId() }
            };
          case LedgerAnchor::Type::LEGACY_GRADIDO_DB_CONTRIBUTION_ID:
            return {
              .type = GRDW_LEDGER_ANCHOR_TYPE_LEGACY_GRADIDO_DB_CONTRIBUTION_ID,
              .anchor_id = { .legacy_contribution_id = ledgerAnchor.getLegacyTransactionId() }
            };
          case LedgerAnchor::Type::LEGACY_GRADIDO_DB_TRANSACTION_LINK_ID:
            return {
              .type = GRDW_LEDGER_ANCHOR_TYPE_LEGACY_GRADIDO_DB_TRANSACTION_LINK_ID,
              .anchor_id = { .legacy_transaction_link_id = ledgerAnchor.getLegacyTransactionId() }
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
