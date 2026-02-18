#ifndef GRADIDO_BLOCKCHAIN_BLOCKCHAIN_COMMUNITY_ID_TYPE_H
#define GRADIDO_BLOCKCHAIN_BLOCKCHAIN_COMMUNITY_ID_TYPE_H

#include "gradido_blockchain/types.h"

#include <typeinfo>

namespace gradido::blockchain {

  enum class CommunityIdType : uint8_t {
    NONE = 0,
    COIN_COMMUNITY_ID = 1,
    PUBLIC_KEY_COMMUNITY_ID = 2,
    COIN_COMMUNITY_ID_AND_PUBLIC_KEY_COMMUNITY_ID = 3
  };

	constexpr inline CommunityIdType operator | (CommunityIdType lhs, CommunityIdType rhs)
	{
		using T = std::underlying_type_t <CommunityIdType>;
		return static_cast<CommunityIdType>(static_cast<T>(lhs) | static_cast<T>(rhs));
	}

	constexpr inline CommunityIdType operator & (CommunityIdType lhs, CommunityIdType rhs)
	{
		using T = std::underlying_type_t <CommunityIdType>;
		return static_cast<CommunityIdType>(static_cast<T>(lhs) & static_cast<T>(rhs));
	}
	constexpr inline CommunityIdType operator - (CommunityIdType lhs, CommunityIdType rhs)
	{
		using T = std::underlying_type_t <CommunityIdType>;
		return static_cast<CommunityIdType>(static_cast<T>(lhs) - static_cast<T>(rhs));
	}

}

#endif //GRADIDO_BLOCKCHAIN_BLOCKCHAIN_COMMUNITY_ID_TYPE_H
