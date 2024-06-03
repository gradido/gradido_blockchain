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
			if(mProtoRegisterAddress.account_pubkey().size() == crypto_sign_PUBLICKEYBYTES) {
				mRequiredSignPublicKeys.push_back(getAccountPubkey());
			}
			else if (mProtoRegisterAddress.user_pubkey().size() == crypto_sign_PUBLICKEYBYTES) {
			  	mRequiredSignPublicKeys.push_back(getUserPubkey());
			} 
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
					throw WrongAddressTypeException(
						"register address transaction not allowed with community auf or gmw account",
						getAddressType(),
						getUserPubkey()
					);
				}
			}

			if ((level & TRANSACTION_VALIDATION_CONNECTED_GROUP) == TRANSACTION_VALIDATION_CONNECTED_GROUP) {
				assert(blockchain);

				std::shared_ptr<model::TransactionEntry> lastTransaction;

				std::string address;

				switch (getAddressType()) {
				case proto::gradido::RegisterAddress_AddressType_COMMUNITY_HUMAN:
				case proto::gradido::RegisterAddress_AddressType_COMMUNITY_PROJECT:
				case proto::gradido::RegisterAddress_AddressType_CRYPTO_ACCOUNT:
					address = getUserPubkey();
					break;
				case proto::gradido::RegisterAddress_AddressType_SUBACCOUNT:
					address = getAccountPubkey();
					break;
				}
				lastTransaction = blockchain->findLastTransactionForAddress(address);
				if (lastTransaction) {
					throw AddressAlreadyExistException("cannot register address because it already exist", DataTypeConverter::binToHex(address), getAddressType());
				}

			}

			// TODO: check if address wasn't already registered to this blockchain
			return true;
		}

		std::vector<std::string_view> RegisterAddress::getInvolvedAddresses() const
		{
			std::vector<std::string_view> result;

			auto userPubkey = getUserPubkey();
			if (userPubkey.size()) {
				result.push_back(userPubkey);
			}
			auto subaccountPubkey = getAccountPubkey();
			if (subaccountPubkey.size()) {
				result.push_back(subaccountPubkey);
			}
			return result;
		}
		bool RegisterAddress::isInvolved(const std::string& pubkeyString) const
		{
			return 
				mProtoRegisterAddress.user_pubkey() == pubkeyString ||
				mProtoRegisterAddress.account_pubkey() == pubkeyString
				;
		}

		bool RegisterAddress::isBelongToUs(const TransactionBase* pairingTransaction) const
		{
			auto pair = dynamic_cast<const RegisterAddress*>(pairingTransaction);

			if (getUserPubkey() != pair->getUserPubkey() ||
				getNameHash() != pair->getNameHash() ||
				getAccountPubkey() != pair->getAccountPubkey() ||
				getAddressType() != pair->getAddressType() ||
				getDerivationIndex() != pair->getDerivationIndex()) {
				return false;
			}

			return true;
		}

		const std::string& RegisterAddress::getUserPubkey() const
		{
			// cannot inline, because this doens't work in dll build
			return mProtoRegisterAddress.user_pubkey();
		}

		proto::gradido::RegisterAddress_AddressType RegisterAddress::getAddressType() const
		{
			// cannot inline, because this doens't work in dll build
			return mProtoRegisterAddress.address_type();
		}

		const std::string& RegisterAddress::getNameHash() const
		{
			// cannot inline, because this doens't work in dll build
			return mProtoRegisterAddress.name_hash();
		}

		const std::string& RegisterAddress::getAccountPubkey() const
		{
			// cannot inline, because this doens't work in dll build
			return mProtoRegisterAddress.account_pubkey();
		}

		uint32_t RegisterAddress::getDerivationIndex() const
		{
			return mProtoRegisterAddress.derivation_index();
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
