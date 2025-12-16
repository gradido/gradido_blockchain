#ifndef __GRADIDO_BLOCKCHAIN_DATA_BALANCE_DERIVATION_TYPE_H
#define __GRADIDO_BLOCKCHAIN_DATA_BALANCE_DERIVATION_TYPE_H

#include <stdint.h>

namespace gradido {
	namespace data {
		/*!
		* @enum model::gradido::BalanceDerivationType
		* Flag to decide if Node calculate balance and decay or trust extern data
		*/
		enum class BalanceDerivationType : uint8_t {
			UNSPECIFIED = 0,
			// Balances & decay can be recalculated deterministically
			NODE = 1,
			// Balances are accepted as-is from external / legacy system
			EXTERN = 2
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_DATA_BALANCE_DERIVATION_TYPE_H