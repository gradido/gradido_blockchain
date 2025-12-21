#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_LEDGER_ANCHOR_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_LEDGER_ANCHOR_ROLE_H

#include "Protopuf.h"
#include "gradido_blockchain/data/LedgerAnchor.h"

namespace gradido {
    namespace interaction {
        namespace deserialize {
            class LedgerAnchorRole
            {
            public:
                LedgerAnchorRole(const LedgerAnchorMessage& ledgerAnchor);
                inline operator const data::LedgerAnchor& () const { return mLedgerAnchor; }
                inline const data::LedgerAnchor& data() const { return mLedgerAnchor; }
            protected:
                data::LedgerAnchor mLedgerAnchor;
            };
        }
    }
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_LEDGER_ANCHOR_ROLE_H