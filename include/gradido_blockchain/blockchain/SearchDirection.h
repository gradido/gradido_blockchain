#ifndef __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_SEARCH_DIRECTION_H
#define __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_SEARCH_DIRECTION_H

namespace gradido {
	namespace blockchain {

		enum class SearchDirection : uint8_t
		{
			//! ascending order
			ASC,
			//! descending order
			DESC
		};
	}
}

#endif // __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_SEARCH_DIRECTION_H