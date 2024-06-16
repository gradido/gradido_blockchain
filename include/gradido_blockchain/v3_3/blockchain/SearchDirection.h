#ifndef __GRADIDO_BLOCKCHAIN_V3_3_BLOCKCHAIN_SEARCH_DIRECTION_H
#define __GRADIDO_BLOCKCHAIN_V3_3_BLOCKCHAIN_SEARCH_DIRECTION_H

namespace gradido {
	namespace v3_3 {
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
}

#endif // __GRADIDO_BLOCKCHAIN_V3_3_BLOCKCHAIN_SEARCH_DIRECTION_H