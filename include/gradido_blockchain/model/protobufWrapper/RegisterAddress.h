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
			bool isInvolved(const std::string pubkeyString) const;
			bool isBelongToUs(const TransactionBase* pairingTransaction) const;

			MemoryBin* getUserPubkey() const;
			const std::string& getUserPubkeyString() const;
			proto::gradido::RegisterAddress_AddressType getAddressType() const;
			inline bool isSubaccount() const { return getAddressType() == proto::gradido::RegisterAddress_AddressType_SUBACCOUNT;}
			inline bool isHuman() const { return getAddressType() == proto::gradido::RegisterAddress_AddressType_HUMAN; }
			MemoryBin* getNameHash() const;
			const std::string& getNameHashString() const;
			MemoryBin* getSubaccountPubkey() const;
			const std::string& getSubaccountPubkeyString() const;

			static proto::gradido::RegisterAddress_AddressType getAddressTypeFromString(const std::string& addressType);
			static const std::string& getAddressStringFromType(proto::gradido::RegisterAddress_AddressType type);

		protected:
			const proto::gradido::RegisterAddress& mProtoRegisterAddress;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_REGISTER_ADDRESS_H
