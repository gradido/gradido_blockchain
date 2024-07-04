#ifndef __GRADIDO_BLOCKCHAIN_FILTER_TYPE_H
#define __GRADIDO_BLOCKCHAIN_FILTER_TYPE_H

#include <typeinfo>

namespace gradido {
	namespace blockchain {
		
		//! One bit Filter Type so filter types can be combined in one variable
		enum class FilterCriteria: uint16_t {
			TRANSACTION_NR = 1UL << 0,
			INVOLVED_PUBLIC_KEY = 1UL << 1,
			COIN_COMMUNITY = 1UL << 2,
			TIMEPOINT_INTERVAL = 1UL << 3,
			FILTER_FUNCTION = 1UL << 4,
			TRANSACTION_TYPE = 1UL << 5,
			MAX = (1UL << 6) - 1
		};

		inline FilterCriteria operator | (FilterCriteria lhs, FilterCriteria rhs)
		{
			using T = std::underlying_type_t <FilterCriteria>;
			return static_cast<FilterCriteria>(static_cast<T>(lhs) | static_cast<T>(rhs));
		}

		inline FilterCriteria operator & (FilterCriteria lhs, FilterCriteria rhs)
		{
			using T = std::underlying_type_t <FilterCriteria>;
			return static_cast<FilterCriteria>(static_cast<T>(lhs) & static_cast<T>(rhs));
		}
		inline FilterCriteria operator - (FilterCriteria lhs, FilterCriteria rhs)
		{
			using T = std::underlying_type_t <FilterCriteria>;
			return static_cast<FilterCriteria>(static_cast<T>(lhs) - static_cast<T>(rhs));
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_FILTER_TYPE_H