#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_LEDGER_ANCHOR_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_LEDGER_ANCHOR_ROLE_H

#include "gradido_blockchain/export.h"
#include "AbstractRole.h"
#include "Protopuf.h"

namespace gradido {
	namespace data {
		class LedgerAnchor;
	}
	namespace interaction {
		namespace serialize {

			class GRADIDOBLOCKCHAIN_EXPORT LedgerAnchorRole : public AbstractRole
			{
			public:
				LedgerAnchorRole(const data::LedgerAnchor& ledgerAnchor) : mLedgerAnchor(ledgerAnchor) {}
				~LedgerAnchorRole() {};

				RUM_IMPLEMENTATION
				LedgerAnchorMessage getMessage() const;
				size_t calculateSerializedSize() const;
				// new structure
				static size_t serializedSize(const data::LedgerAnchor& ledgerAnchor);

			protected:
				const data::LedgerAnchor& mLedgerAnchor;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_LEDGER_ANCHOR_ROLE_H
