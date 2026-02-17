#ifndef GRADIDO_BLOCKCHAIN_BLOCKCHAIN_COMPACT_PAGINATION_H
#define GRADIDO_BLOCKCHAIN_BLOCKCHAIN_COMPACT_PAGINATION_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/types.h"

#include "Pagination.h"

#include <cstdint>

namespace gradido {
	namespace blockchain {

		struct GRADIDOBLOCKCHAIN_EXPORT CompactPagination
		{
			CompactPagination()
				: size(0), page(0) {
			}
			CompactPagination(const Pagination& other)
				: size(other.size), page(other.page) {
				if (other.size != (uint16_t)size || other.page != (uint16_t)page) {
					throw GradidoNodeInvalidDataException("cannot use CompactPagination when size or page exceed uint16");
				}
			}
			CompactPagination(uint16_t _size)
				: size(_size), page(0) {}
			CompactPagination(uint16_t _size, uint16_t _page)
				: size(_size), page(_page) {}

			//! page size, how many result should be returned at max
			//! offset if requesting other than page = 0
			//! return all for 0
			uint16_t size;
			uint16_t page;

			//! return entries to skip, only if page > 0, else return 0
			inline unsigned int skipEntriesCount() const {
				if (!page) return 0;
				return (page-1) * size;
			}
			inline bool hasCapacityLeft(uint16_t currentCount) const {
				if (!size) return true;
				if (currentCount >= size) return false;
				return true;
			}
			inline bool operator==(const CompactPagination& other) const {
				return size == other.size && page == other.page;
			}
			inline bool operator !=(const CompactPagination& other) const {
				return !(*this == other);
			}
		};
	}
}

#endif //GRADIDO_BLOCKCHAIN_BLOCKCHAIN_COMPACT_PAGINATION_H