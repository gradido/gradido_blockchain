#include "gradido_blockchain/model/protobufWrapper/RegisterAddress.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

namespace model {
	namespace gradido {
		RegisterAddress::RegisterAddress(const proto::gradido::RegisterAddress& registerAddress)
			: mProtoRegisterAddress(registerAddress)
		{

		}

		RegisterAddress::~RegisterAddress()
		{

		}

		int RegisterAddress::prepare()
		{
			mMinSignatureCount = 1;
			return 0;
		}

		bool RegisterAddress::validate(
			TransactionValidationLevel level/* = TRANSACTION_VALIDATION_SINGLE*/, 
			IGradidoBlockchain* blockchain/* = nullptr*/,
			const GradidoBlock* parentGradidoBlock/* = nullptr*/) const
		{
			if ((level & TRANSACTION_VALIDATION_SINGLE) == TRANSACTION_VALIDATION_SINGLE) {

			}
			return true;
		}

		std::vector<MemoryBin*> RegisterAddress::getInvolvedAddresses() const 
		{
			auto mm = MemoryManager::getInstance();
			std::vector<MemoryBin*> result;

			auto userPubkeySize = mProtoRegisterAddress.user_pubkey().size();			
			if (userPubkeySize) {
				auto userPubkey = mm->getMemory(userPubkeySize);
				memcpy(*userPubkey, mProtoRegisterAddress.user_pubkey().data(), userPubkeySize);
				result.push_back(userPubkey);
			}
			auto subaccountPubkeySize = mProtoRegisterAddress.subaccount_pubkey().size();
			if (subaccountPubkeySize) {
				auto subaccountPubkey = mm->getMemory(subaccountPubkeySize);
				memcpy(*subaccountPubkey, mProtoRegisterAddress.subaccount_pubkey().data(), subaccountPubkeySize);
				result.push_back(subaccountPubkey);
			}
			return result;
		}

		uint32_t RegisterAddress::getCoinColor() const
		{
			return 0;
		}

		bool RegisterAddress::isBelongToUs(const TransactionBase* pairingTransaction) const
		{
			auto pair = dynamic_cast<const RegisterAddress*>(pairingTransaction);
			auto mm = MemoryManager::getInstance();
			bool belongToUs = true;

			auto a = getUserPubkey();
			auto b = pair->getUserPubkey();			
			if (!a->isSame(b)) {
				belongToUs = false;
			}
			mm->releaseMemory(a); mm->releaseMemory(b);

			a = getNameHash();
			b = pair->getNameHash();
			if (!a->isSame(b)) {
				belongToUs = false;
			}
			mm->releaseMemory(a); mm->releaseMemory(b);

			a = getSubaccountPubkey();
			b = pair->getSubaccountPubkey();
			if (!a->isSame(b)) {
				belongToUs = false;
			}
			mm->releaseMemory(a); mm->releaseMemory(b);
			
			return belongToUs;
		}

		MemoryBin* RegisterAddress::getUserPubkey() const
		{
			auto bin = MemoryManager::getInstance()->getMemory(mProtoRegisterAddress.user_pubkey().size());
			bin->copyFromProtoBytes(mProtoRegisterAddress.user_pubkey());
			return bin;
		}

		MemoryBin* RegisterAddress::getNameHash() const
		{
			auto bin = MemoryManager::getInstance()->getMemory(mProtoRegisterAddress.name_hash().size());
			bin->copyFromProtoBytes(mProtoRegisterAddress.name_hash());
			return bin;
		}

		MemoryBin* RegisterAddress::getSubaccountPubkey() const
		{
			auto bin = MemoryManager::getInstance()->getMemory(mProtoRegisterAddress.subaccount_pubkey().size());
			bin->copyFromProtoBytes(mProtoRegisterAddress.subaccount_pubkey());
			return bin;
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