#ifndef __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_FILTER_RESULT_H
#define __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_FILTER_RESULT_H

namespace gradido {	
	namespace blockchain {

		enum class FilterResult : uint8_t
		{
			//! put transaction in result 
			USE = 1,
			//! don't put transaction in result
			DISMISS = 2,
			//! stop search and return with result
			STOP = 4
		};

		inline FilterResult operator | (FilterResult lhs, FilterResult rhs)
		{
			using T = std::underlying_type_t <FilterResult>;
			return static_cast<FilterResult>(static_cast<T>(lhs) | static_cast<T>(rhs));
		}

		inline FilterResult operator & (FilterResult lhs, FilterResult rhs)
		{
			using T = std::underlying_type_t <FilterResult>;
			return static_cast<FilterResult>(static_cast<T>(lhs) & static_cast<T>(rhs));
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_BLOCKCHAIN_FILTER_RESULT_H