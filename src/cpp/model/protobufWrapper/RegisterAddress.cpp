#include "RegisterAddress.h"

namespace model {
	namespace gradido {
		RegisterAddress::RegisterAddress(const proto::gradido::RegisterAddress& registerAddress)
			: mRegisterAddress(registerAddress)
		{

		}

		RegisterAddress::~RegisterAddress()
		{

		}

		bool RegisterAddress::validate(TransactionValidationLevel level/* = TRANSACTION_VALIDATION_SINGLE*/, IGradidoBlockchain* blockchain/* = nullptr*/) const
		{
			if ((level & TRANSACTION_VALIDATION_SINGLE) == TRANSACTION_VALIDATION_SINGLE) {

			}
			return true;
		}

	}
}