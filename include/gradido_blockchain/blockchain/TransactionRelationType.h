#ifndef __GRADIDO_BLOCKCHAIN_TRANSACTION_RELATION_TYPE_H
#define __GRADIDO_BLOCKCHAIN_TRANSACTION_RELATION_TYPE_H

#include <inttypes.h>

namespace gradido {
	namespace blockchain {
		enum class TransactionRelationType: uint8_t {
			//! previous transaction from sender
			SenderPrevious,
			//! previous transaction from recipipent
			RecipientPrevious,
			//! previous transaction from blockchain, previous transaction nr
			Previous,
			//! previous transaction which changed gmw account balance
			GmwPrevious,
			//! previous transaction which changed auf account balance
			AufPrevious
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_TRANSACTION_RELATION_TYPE_H