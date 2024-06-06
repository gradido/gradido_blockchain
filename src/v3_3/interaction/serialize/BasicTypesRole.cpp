#include "gradido_blockchain/v3_3/interaction/serialize/BasicTypesRole.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace serialize {

				TimestampMessage BasicTypeRoles::getTimestampMessage(const data::Timestamp& timestamp)
				{
					return TimestampMessage{
						timestamp.seconds,
						timestamp.nanos
					};
				}

			}
		}
	}
}
