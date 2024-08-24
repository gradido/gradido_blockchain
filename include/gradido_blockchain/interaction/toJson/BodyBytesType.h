#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_TO_JSON_BODY_BYTES_TYPE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_TO_JSON_BODY_BYTES_TYPE_H

#include <type_traits>

namespace gradido {
	namespace interaction {
		namespace toJson {
			enum class BodyBytesType
			{
				BASE64 = 1,
				HEX = 2,
				JSON = 4
			};
			inline BodyBytesType operator | (BodyBytesType lhs, BodyBytesType rhs)
			{
				using T = std::underlying_type_t <BodyBytesType>;
				return static_cast<BodyBytesType>(static_cast<T>(lhs) | static_cast<T>(rhs));
			}

			inline BodyBytesType operator & (BodyBytesType lhs, BodyBytesType rhs)
			{
				using T = std::underlying_type_t <BodyBytesType>;
				return static_cast<BodyBytesType>(static_cast<T>(lhs) & static_cast<T>(rhs));
			}
		}
	}
}


#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_TO_JSON_BODY_BYTES_TYPE_H