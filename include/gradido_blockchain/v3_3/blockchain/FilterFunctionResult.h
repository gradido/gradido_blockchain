#ifndef __GRADIDO_BLOCKCHAIN_V3_3_BLOCKCHAIN_FILTER_FUNCTION_RESULT_H
#define __GRADIDO_BLOCKCHAIN_V3_3_BLOCKCHAIN_FILTER_FUNCTION_RESULT_H

namespace gradido {
	namespace v3_3 {
		namespace blockchain {

			enum class FilterFunctionResult : uint8_t
			{
				//! put transaction in result 
				USE = 1,
				//! don't put transaction in result
				DISMISS = 2,
				//! stop search and return with result
				STOP = 4
			};

			inline FilterFunctionResult operator | (FilterFunctionResult lhs, FilterFunctionResult rhs)
			{
				using T = std::underlying_type_t <FilterFunctionResult>;
				return static_cast<FilterFunctionResult>(static_cast<T>(lhs) | static_cast<T>(rhs));
			}

			inline FilterFunctionResult operator & (FilterFunctionResult lhs, FilterFunctionResult rhs)
			{
				using T = std::underlying_type_t <FilterFunctionResult>;
				return static_cast<FilterFunctionResult>(static_cast<T>(lhs) & static_cast<T>(rhs));
			}
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_V3_3_BLOCKCHAIN_FILTER_FUNCTION_RESULT_H