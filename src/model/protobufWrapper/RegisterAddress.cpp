#include "gradido_blockchain/model/protobufWrapper/RegisterAddress.h"
#include "gradido_blockchain/model/IGradidoBlockchain.h"
#include "gradido_blockchain/model/protobufWrapper/TransactionValidationExceptions.h"

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
			const ConfirmedTransaction* parentGradidoBlock/* = nullptr*/) const
		{
			if ((level & TRANSACTION_VALIDATION_SINGLE) == TRANSACTION_VALIDATION_SINGLE) {
				if (isCommunityGMW() || isCommunityAUF()) {
					throw WrongAddressTypeException("register address transaction not allowed with community auf or gmw account", getAddressType(), getUserPubkeyString());
				}
			}

			if ((level & TRANSACTION_VALIDATION_CONNECTED_GROUP) == TRANSACTION_VALIDATION_CONNECTED_GROUP) {
				assert(blockchain);

				Poco::SharedPtr<model::TransactionEntry> lastTransaction;

				std::string address;

				switch (getAddressType()) {
				case proto::gradido::RegisterAddress_AddressType_COMMUNITY_HUMAN:
				case proto::gradido::RegisterAddress_AddressType_COMMUNITY_PROJECT:
				case proto::gradido::RegisterAddress_AddressType_CRYPTO_ACCOUNT:
					address = getUserPubkeyString();
					break;
				case proto::gradido::RegisterAddress_AddressType_SUBACCOUNT:
					address = getSubaccountPubkeyString();
					break;
				}
				lastTransaction = blockchain->findLastTransactionForAddress(address);
				if (!lastTransaction.isNull()) {
					throw AddressAlreadyExistException("cannot register address because it already exist", DataTypeConverter::binToHex(address), getAddressType());
				}

			}

			// TODO: check if address wasn't already registered to this blockchain
			return true;
		}

		std::vector<MemoryBin*> RegisterAddress::getInvolvedAddresses() const
		{
			auto mm = MemoryManager::getInstance();
			std::vector<MemoryBin*> result;

			auto userPubkeySize = mProtoRegisterAddress.user_pubkey().size();
			auto userPubkey = getUserPubkey();
			if (userPubkey) {
				result.push_back(userPubkey);
			}
			auto subaccountPubkey = getSubaccountPubkey();
			if (subaccountPubkey) {
				result.push_back(subaccountPubkey);
			}
			return result;
		}
		bool RegisterAddress::isInvolved(const std::string pubkeyString) const
		{
			return 
				mProtoRegisterAddress.user_pubkey() == pubkeyString ||
				mProtoRegisterAddress.subaccount_pubkey() == pubkeyString
				;
		}

		bool RegisterAddress::isBelongToUs(const TransactionBase* pairingTransaction) const
		{
			auto pair = dynamic_cast<const RegisterAddress*>(pairingTransaction);

			// std::string::compare return 0 if strings identical else < 0 or > 0
			if (getUserPubkeyString().compare(pair->getUserPubkeyString())) {
				return false;
			}
			if (getNameHashString().compare(pair->getNameHashString())) {
				return false;
			}
			if (getSubaccountPubkeyString().compare(pair->getUserPubkeyString())) {
				return false;
			}

			return true;
		}

		const std::string& RegisterAddress::getUserPubkeyString() const
		{
			// cannot inline, because this doens't work in dll build
			return mProtoRegisterAddress.user_pubkey();
		}

		proto::gradido::RegisterAddress_AddressType RegisterAddress::getAddressType() const
		{
			// cannot inline, because this doens't work in dll build
			return mProtoRegisterAddress.address_type();
		}

		const std::string& RegisterAddress::getNameHashString() const
		{
			// cannot inline, because this doens't work in dll build
			return mProtoRegisterAddress.name_hash();
		}

		const std::string& RegisterAddress::getSubaccountPubkeyString() const
		{
			// cannot inline, because this doens't work in dll build
			return mProtoRegisterAddress.subaccount_pubkey();
		}

		proto::gradido::RegisterAddress_AddressType RegisterAddress::getAddressTypeFromString(const std::string& addressType)
		{
			proto::gradido::RegisterAddress_AddressType type;
			if (!proto::gradido::RegisterAddress_AddressType_Parse(addressType, &type)) {
				throw GradidoInvalidEnumException("getAddressTypeFromString", addressType);
			}

			return type;
		}
		const std::string& RegisterAddress::getAddressStringFromType(proto::gradido::RegisterAddress_AddressType type)
		{
			// cannot inline, because this doens't work in dll build
			return RegisterAddress_AddressType_Name(type);
		}

		std::string RegisterAddress::toDebugString() const 
		{
			std::string result;
			result += "register address\n";
			return std::move(result);
		}


	}
}
