
#ifndef __GRADIDO_BLOCKCHAIN_V3_3_BLOCKCHAIN_PAGINATION_H
#define __GRADIDO_BLOCKCHAIN_V3_3_BLOCKCHAIN_PAGINATION_H

#include "gradido_blockchain/export.h"
#include <cstdint>

namespace gradido {
	namespace v3_3 {
		namespace blockchain {

			struct GRADIDOBLOCKCHAIN_EXPORT Pagination
			{
				Pagination(unsigned int _size) 
					: size(_size), page(0) {}
				Pagination(unsigned int _size, unsigned int _page)
					: size(_size), page(_page) {}

				//! page size, how many result should be returned at max
				//! offset if requesting other than page = 0
				unsigned int size;

				unsigned int page;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_V3_3_BLOCKCHAIN_PAGINATION_H