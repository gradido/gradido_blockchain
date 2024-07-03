#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_ABSTRACT_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_ABSTRACT_ROLE_H

#include "Exceptions.h"

#include <optional>

namespace gradido {
	namespace interaction {
		namespace deserialize {
			class AbstractRole
			{
			protected:
				template<class T>
				void checkType(const std::optional<T>& message, const char* fieldName, const char* functionName = __FUNCTION__) {
					if (!message["created_at"_f].has_value()) {
						throw MissingMemberException("missing member on deserialize transaction body", "created_at");
					}
				}
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_ABSTRACT_ROLE_H