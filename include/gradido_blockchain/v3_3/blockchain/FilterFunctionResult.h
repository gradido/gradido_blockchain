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
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_V3_3_BLOCKCHAIN_FILTER_FUNCTION_RESULT_H