#include "gradido_blockchain/model/protobufWrapper/RegisterAddress.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

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

		std::vector<MemoryBin*> RegisterAddress::getInvolvedAddresses() const 
		{
			auto mm = MemoryManager::getInstance();
			std::vector<MemoryBin*> result;

			auto userPubkeySize = mRegisterAddress.user_pubkey().size();			
			if (userPubkeySize) {
				auto userPubkey = mm->getMemory(userPubkeySize);
				memcpy(*userPubkey, mRegisterAddress.user_pubkey().data(), userPubkeySize);
				result.push_back(userPubkey);
			}
			auto subaccountPubkeySize = mRegisterAddress.subaccount_pubkey().size();
			if (subaccountPubkeySize) {
				auto subaccountPubkey = mm->getMemory(subaccountPubkeySize);
				memcpy(*subaccountPubkey, mRegisterAddress.subaccount_pubkey().data(), subaccountPubkeySize);
				result.push_back(subaccountPubkey);
			}
			return result;
		}

		uint32_t RegisterAddress::getCoinColor() const
		{
			return 0;
		}

		int RegisterAddress::prepare()
		{
			mMinSignatureCount = 1;
			return 0;
		}

		proto::gradido::RegisterAddress_AddressType RegisterAddress::getAddressTypeFromString(const std::string& addressType) 
		{
			proto::gradido::RegisterAddress_AddressType type;
			if (!proto::gradido::RegisterAddress_AddressType_Parse(addressType, &type)) {
				throw GradidoInvalidEnumException("getAddressTypeFromString", addressType);
			}

			return type;
		}

	}
}