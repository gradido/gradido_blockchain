#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_HIERO_TRANSACTION_ID_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_HIERO_TRANSACTION_ID_ROLE_H

#include "Protopuf.h"
#include "gradido_blockchain/data/hiero/TransactionId.h"

namespace gradido {
	namespace interaction {
		namespace deserialize {

            class HieroTransactionIdRole
            {
            public:
                HieroTransactionIdRole(const HieroTransactionIdMessage& hieroTransactionId);
                ~HieroTransactionIdRole();

                inline operator const hiero::TransactionId& () const { return mTransactionId; }
                inline const hiero::TransactionId& data() const { return mTransactionId; }
            protected:
                hiero::TransactionId mTransactionId;
            };
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_HIERO_TRANSACTION_ID_ROLE_H
