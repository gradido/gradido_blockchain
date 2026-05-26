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
                else if (ledgerAnchor["id"_f].has_value()) {
                    mLedgerAnchor = LedgerAnchor(ledgerAnchor["id"_f].value(), ledgerAnchor["type"_f].value());
                } 
            }
        }
    }
}