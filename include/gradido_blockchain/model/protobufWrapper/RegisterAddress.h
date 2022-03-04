#ifndef __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_REGISTER_ADDRESS_H
#define __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_REGISTER_ADDRESS_H

#include "proto/gradido/RegisterAddress.pb.h"
#include "TransactionBase.h"

namespace model {
	namespace gradido {

		class GRADIDOBLOCKCHAIN_EXPORT RegisterAddress : public TransactionBase
		{
		public:
			RegisterAddress(const proto::gradido::RegisterAddress& registerAddress);
			~RegisterAddress();

			int prepare();
			bool validate(
				TransactionValidationLevel level = TRANSACTION_VALIDATION_SINGLE,
				IGradidoBlockchain* blockchain = nullptr,
				const GradidoBlock* parentGradidoBlock = nullptr
			) const;
			std::vector<MemoryBin*> getInvolvedAddresses() const;
			uint32_t getCoinColor() const;
			bool isBelongToUs(const TransactionBase* pairingTransaction) const;

			MemoryBin* getUserPubkey() const;
			inline const std::string& getUserPubkeyString() const { return mProtoRegisterAddress.user_pubkey(); }
			proto::gradido::RegisterAddress_AddressType getAddressType() const { return mProtoRegisterAddress.address_type(); }
			MemoryBin* getNameHash() const;
			inline const std::string& getNameHashString() const { return mProtoRegisterAddress.name_hash(); }
			MemoryBin* getSubaccountPubkey() const;
			inline const std::string& getSubaccountPubkeyString() const { return mProtoRegisterAddress.subaccount_pubkey(); }

			static proto::gradido::RegisterAddress_AddressType getAddressTypeFromString(const std::string& addressType);
			inline static const std::string& getAddressStringFromType(proto::gradido::RegisterAddress_AddressType type) {
				return RegisterAddress_AddressType_Name(type);
			}

		protected:
			const proto::gradido::RegisterAddress& mProtoRegisterAddress;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_REGISTER_ADDRESS_H