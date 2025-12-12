
#ifndef __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_PAGINATION_H
#define __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_PAGINATION_H

#include "gradido_blockchain/export.h"
#include <cstdint>

namespace gradido {
	namespace blockchain {

		struct GRADIDOBLOCKCHAIN_EXPORT Pagination
		{
			Pagination()
				: size(0), page(0) {}
			Pagination(unsigned int _size) 
				: size(_size), page(0) {}
			Pagination(unsigned int _size, unsigned int _page)
				: size(_size), page(_page) {}

			//! page size, how many result should be returned at max
			//! offset if requesting other than page = 0
			//! return all for 0
			unsigned int size;

			unsigned int page;

			//! return entries to skip, only if page > 0, else return 0
			inline unsigned int skipEntriesCount() const {
				if (!page) return 0;
				return (page-1) * size;
			}
			inline bool hasCapacityLeft(unsigned int currentCount) const {
				if (!size) return true;
				if (currentCount >= size) return false;
				return true;
			}
			inline bool operator==(const Pagination& other) const {
				return size == other.size && page == other.page;
			}
			inline bool operator !=(const Pagination& other) const {
				return !(*this == other);
			}
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_BLOCKCHAIN_PAGINATION_H