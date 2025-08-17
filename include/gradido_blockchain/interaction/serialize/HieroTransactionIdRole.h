#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_HIERO_TRANSACTION_ID_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_HIERO_TRANSACTION_ID_ROLE_H

#include "AbstractRole.h"
#include "Protopuf.h"

namespace hiero {
	class TransactionId;
}

namespace gradido {
	namespace interaction {
		namespace serialize {

			class HieroTransactionIdRole : public AbstractRole
			{
			public:
				HieroTransactionIdRole(const hiero::TransactionId& transactionId) : mTransactionId(transactionId) {}
				~HieroTransactionIdRole() {};

				RUM_IMPLEMENTATION
				HieroTransactionIdMessage getMessage() const;
				size_t calculateSerializedSize() const;

			protected:
				const hiero::TransactionId& mTransactionId;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_HIERO_TRANSACTION_ID_ROLE_H
