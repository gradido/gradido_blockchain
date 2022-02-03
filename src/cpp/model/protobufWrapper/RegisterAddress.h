#ifndef __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_REGISTER_ADDRESS_H
#define __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_REGISTER_ADDRESS_H

#include "gradido/RegisterAddress.pb.h"
#include "TransactionBase.h"

namespace model {
	namespace gradido {

		class RegisterAddress : public TransactionBase
		{
		public:
			RegisterAddress(const proto::gradido::RegisterAddress& registerAddress);
			~RegisterAddress();

			bool validate(TransactionValidationLevel level = TRANSACTION_VALIDATION_SINGLE, IGradidoBlockchain* blockchain = nullptr) const;

			static proto::gradido::RegisterAddress_AddressType getAddressTypeFromString(const std::string& addressType) {
				proto::gradido::RegisterAddress_AddressType type;
				proto::gradido::RegisterAddress_AddressType_Parse(addressType, &type);
				return type;
			}

		protected:
			const proto::gradido::RegisterAddress& mRegisterAddress;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_REGISTER_ADDRESS_H