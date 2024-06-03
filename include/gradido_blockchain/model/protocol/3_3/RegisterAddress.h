#ifndef __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_REGISTER_ADDRESS_H
#define __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_REGISTER_ADDRESS_H

#include "proto/gradido/register_address.pb.h"
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
				const ConfirmedTransaction* parentGradidoBlock = nullptr
			) const;
			std::vector<std::string_view> getInvolvedAddresses() const;
			bool isInvolved(const std::string& pubkeyString) const;
			bool isBelongToUs(const TransactionBase* pairingTransaction) const;

			const std::string& getUserPubkey() const;

			proto::gradido::RegisterAddress_AddressType getAddressType() const;
			inline bool isCommunityHuman() const { return getAddressType() == proto::gradido::RegisterAddress_AddressType_COMMUNITY_HUMAN; }
			inline bool isCommunityGMW() const { return getAddressType() == proto::gradido::RegisterAddress_AddressType_COMMUNITY_GMW; }
			inline bool isCommunityAUF() const { return getAddressType() == proto::gradido::RegisterAddress_AddressType_COMMUNITY_AUF; }
			inline bool isCommunityProject() const { return getAddressType() == proto::gradido::RegisterAddress_AddressType_COMMUNITY_PROJECT; }
			inline bool isSubaccount() const { return getAddressType() == proto::gradido::RegisterAddress_AddressType_SUBACCOUNT; }
			inline bool isCryptoAccount() const { return getAddressType() == proto::gradido::RegisterAddress_AddressType_CRYPTO_ACCOUNT; }

			const std::string& getNameHash() const;
			const std::string& getAccountPubkey() const;
			uint32_t getDerivationIndex() const;

			static proto::gradido::RegisterAddress_AddressType getAddressTypeFromString(const std::string& addressType);
			static const std::string& getAddressStringFromType(proto::gradido::RegisterAddress_AddressType type);

			std::string toDebugString() const;

		protected:
			const proto::gradido::RegisterAddress& mProtoRegisterAddress;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_REGISTER_ADDRESS_H
