#include "gradido_blockchain/data/hiero/TransactionId.h"
#include "gradido_blockchain/interaction/deserialize/LedgerAnchorRole.h"
#include "gradido_blockchain/interaction/deserialize/HieroTransactionIdRole.h"
#include "gradido_blockchain/memory/Block.h"

using memory::Block;
using hiero::TransactionId;

namespace gradido {
    using data::LedgerAnchor;
    namespace interaction {
        namespace deserialize {
            LedgerAnchorRole::LedgerAnchorRole(const LedgerAnchorMessage& ledgerAnchor)
            {
                if (ledgerAnchor["hiero_transaction_id"_f].has_value()) {
                    mLedgerAnchor = LedgerAnchor(HieroTransactionIdRole(ledgerAnchor["hiero_transaction_id"_f].value()));
                } 
                else if (ledgerAnchor["legacy_transaction_id"_f].has_value()) {
                    mLedgerAnchor = LedgerAnchor(ledgerAnchor["legacy_transaction_id"_f].value(), LedgerAnchor::Type::LEGACY_GRADIDO_DB_TRANSACTION_ID);
                } 
                else if (ledgerAnchor["node_trigger_transaction_id"_f].has_value()) {
                    mLedgerAnchor = LedgerAnchor(ledgerAnchor["node_trigger_transaction_id"_f].value(), LedgerAnchor::Type::NODE_TRIGGER_TRANSACTION_ID);
                } 
                else if (ledgerAnchor["iota_message_id"_f].has_value()) {
                    mLedgerAnchor = LedgerAnchor(ledgerAnchor["iota_message_id"_f].value());
                }
                else if (ledgerAnchor["legacy_community_id"_f].has_value()) {
                    mLedgerAnchor = LedgerAnchor(ledgerAnchor["legacy_community_id"_f].value(), LedgerAnchor::Type::LEGACY_GRADIDO_DB_COMMUNITY_ID);
                }
                else if (ledgerAnchor["legacy_user_id"_f].has_value()) {
                    mLedgerAnchor = LedgerAnchor(ledgerAnchor["legacy_user_id"_f].value(), LedgerAnchor::Type::LEGACY_GRADIDO_DB_USER_ID);
                }
                else if (ledgerAnchor["legacy_contribution_id"_f].has_value()) {
                    mLedgerAnchor = LedgerAnchor(ledgerAnchor["legacy_contribution_id"_f].value(), LedgerAnchor::Type::LEGACY_GRADIDO_DB_CONTRIBUTION_ID);
                }
                else if (ledgerAnchor["legacy_transaction_link_id"_f].has_value()) {
                    mLedgerAnchor = LedgerAnchor(ledgerAnchor["legacy_transaction_link_id"_f].value(), LedgerAnchor::Type::LEGACY_GRADIDO_DB_TRANSACTION_LINK_ID);
                }
            }
        }
    }
}