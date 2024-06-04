#ifndef __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_SERIALIZE_TYPE_H
#define __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_SERIALIZE_TYPE_H

#include "gradido_blockchain/export.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace serialize {
				
				enum class Type {
					UNKNOWN,
					TRANSACTION_BODY,
					GRADIDO_TRANSACTION,
					CONFIRMED_TRANSACTION,
					MAX
				};

			}
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_SERIALIZE_TYPE_H