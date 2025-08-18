#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_TYPE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_TYPE_H

#include "gradido_blockchain/export.h"

namespace gradido {
	namespace interaction {
		namespace deserialize {

			enum class Type {
				UNKNOWN,
				TRANSACTION_BODY,
				GRADIDO_TRANSACTION,
				CONFIRMED_TRANSACTION,
				TRANSACTION_TRIGGER_EVENT,
				HIERO_ACCOUNT_ID,
				HIERO_TRANSACTION_ID,
				MAX
			};

		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_SERIALIZE_TYPE_H