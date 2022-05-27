#include "gradido_blockchain/model/protobufWrapper/TransactionValidationExceptions.h"

#include <sstream>

#include "gradido_blockchain/lib/DataTypeConverter.h"

#include "gradido_blockchain/model/protobufWrapper/GradidoTransaction.h"

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;

namespace model {
	namespace gradido {

		TransactionValidationException::TransactionValidationException(const char* what) noexcept
			: GradidoBlockchainException (what), mType(TRANSACTION_NONE)
		{

		}

		std::string TransactionValidationException::getFullString() const noexcept
		{
			return what();
		}

		TransactionValidationException& TransactionValidationException::setTransactionBody(const TransactionBody* transactionBody)
		{
			mTransactionMemo = transactionBody->getMemo();
			mType = transactionBody->getTransactionType();
			return *this;
		}

		//************* Invalid Input *******************

		TransactionValidationInvalidInputException::TransactionValidationInvalidInputException(
			const char* what, const char* fieldname, const char* fieldType/* = nullptr*/
		) noexcept
			: TransactionValidationException(what), mFieldName(fieldname)
		{
			if (fieldType) {
				mFieldType = fieldType;
			}
		}

		std::string TransactionValidationInvalidInputException::getFullString() const noexcept
		{
			std::string result;
			auto whatString = what();
			result.reserve(mFieldName.size() + mFieldType.size() + strlen(whatString) + 10);
			result = whatString;
			result += " with " + mFieldName + ": " + mFieldType;
			return result;
		}
	    Value TransactionValidationInvalidInputException::getDetails(Document::AllocatorType& alloc) const
		{
			Value detailsObjs(kObjectType);
			detailsObjs.AddMember("what", Value(what(), alloc), alloc);
			detailsObjs.AddMember("fieldName", Value(mFieldName.data(), alloc), alloc);
			detailsObjs.AddMember("fieldType", Value(mFieldType.data(), alloc), alloc);
			return std::move(detailsObjs);
		}

		//************* Invalid Signature *******************

		TransactionValidationInvalidSignatureException::TransactionValidationInvalidSignatureException(
			const char* what, const std::string& pubkey, const std::string& signature, const std::string& bodyBytes/* = ""*/
		) noexcept
			: TransactionValidationException(what), mPubkey(pubkey), mSignature(signature), mBodyBytes(bodyBytes)
		{
		}

		TransactionValidationInvalidSignatureException::~TransactionValidationInvalidSignatureException()
		{
		}

		std::string TransactionValidationInvalidSignatureException::getFullString() const noexcept
		{
			std::string pubkeyHex = getPubkeyHex();
			std::string signatureHex = getSignatureHex();
			std::string bodyBytesBase64 = getBodyBytesBase64();
			auto whatString = what();
			std::string result;
			
			size_t staticTextSize = 29;
			if (mBodyBytes.size()) {
				staticTextSize += 14;
			}
			result.reserve(pubkeyHex.size() + signatureHex.size() + bodyBytesBase64.size() + strlen(whatString) + staticTextSize);
			result = whatString;
			result += " with pubkey: " + pubkeyHex.substr(0, 64);
			result += ", signature: " + signatureHex.substr(0, 64);
			if (mBodyBytes.size()) {
				result += ", body bytes: " + bodyBytesBase64;
			}
			return result;
		}

		Value TransactionValidationInvalidSignatureException::getDetails(Document::AllocatorType& alloc) const
		{
			Value detailsObjs(kObjectType);
			detailsObjs.AddMember("what", Value(what(), alloc), alloc);
			detailsObjs.AddMember("pubkey", Value(getPubkeyHex().data(), alloc), alloc);
			detailsObjs.AddMember("signature", Value(getSignatureHex().data(), alloc), alloc);
			detailsObjs.AddMember("bodyBytes", Value(getBodyBytesBase64().data(), alloc), alloc);
			return std::move(detailsObjs);
		}

		std::string TransactionValidationInvalidSignatureException::getPubkeyHex() const noexcept
		{
			if (mPubkey.size()) {
				try {
					return std::move(DataTypeConverter::binToHex(mPubkey));
				}
				catch (...) {}
			}
			return "";
		}
		std::string TransactionValidationInvalidSignatureException::getSignatureHex() const noexcept
		{
			if (mSignature.size()) {
				try {
					return std::move(DataTypeConverter::binToHex(mSignature));
				}
				catch (...) {}
			}
			return "";
		}
		std::string TransactionValidationInvalidSignatureException::getBodyBytesBase64() const noexcept
		{
			if (mBodyBytes.size()) {
				try {
					return std::move(DataTypeConverter::binToBase64(mBodyBytes));
				}
				catch (...) {}
			}
			return "";
		}

		// ************* Forbidden Sign *******************
		TransactionValidationForbiddenSignException::TransactionValidationForbiddenSignException(const std::string& forbiddenPubkey) noexcept
			: TransactionValidationException("Forbidden Sign"), mForbiddenPubkey(forbiddenPubkey)
		{
		}
		TransactionValidationForbiddenSignException::~TransactionValidationForbiddenSignException()
		{

		}

		std::string TransactionValidationForbiddenSignException::getFullString() const noexcept
		{
			std::string forbiddenPubkeyHex = getForbiddenPubkeyHex();
			size_t resultSize = 0;
			if (mTransactionMemo.size()) {
				resultSize += mTransactionMemo.size() + 25;
			}
			if (forbiddenPubkeyHex.size()) {
				resultSize += forbiddenPubkeyHex.size() + 47;
			}
			std::string result;
			result.reserve(resultSize);
			if (mTransactionMemo.size()) {
				result += "transaction with memo: " + mTransactionMemo;
			}
			if (forbiddenPubkeyHex.size()) {
				result += ", this forbidden pubkey was used for signing: " + forbiddenPubkeyHex;
			}
			return result;
		}

		Value TransactionValidationForbiddenSignException::getDetails(Document::AllocatorType& alloc) const
		{
			Value detailsObjs(kObjectType);
			detailsObjs.AddMember("what", Value(what(), alloc), alloc);
			detailsObjs.AddMember("forbiddenPubkey", Value(getForbiddenPubkeyHex().data(), alloc), alloc);
			detailsObjs.AddMember("memo", Value(mTransactionMemo.data(), alloc), alloc);
			return std::move(detailsObjs);
		}

		std::string TransactionValidationForbiddenSignException::getForbiddenPubkeyHex() const noexcept
		{
			if (mForbiddenPubkey.size()) {
				try {
					return std::move(DataTypeConverter::binToHex(mForbiddenPubkey));
				}
				catch (...) {}
			}
			return "";
		}

		// ********************************* Missing Sign ************************************************************
		TransactionValidationMissingSignException::TransactionValidationMissingSignException(int currentSignCount, int requiredSignCount) noexcept
			: TransactionValidationException("missing sign"), mCurrentSignCount(currentSignCount), mRequiredSignCount(requiredSignCount)
		{

		}

		std::string TransactionValidationMissingSignException::getFullString() const noexcept
		{
			size_t resultSize = 12; // missing sign
			auto currentSignCountString = std::to_string(mCurrentSignCount);
			auto requiredSignCountString = std::to_string(mRequiredSignCount);
			resultSize += currentSignCountString.size() + requiredSignCountString.size() + 10 + 6 + 2;

			std::string result;
			result.reserve(resultSize);
			result = what();
			result += ", signed: " + currentSignCountString + " from " + requiredSignCountString;
			return result;
		}

		Value TransactionValidationMissingSignException::getDetails(Document::AllocatorType& alloc) const
		{
			Value detailsObjs(kObjectType);
			detailsObjs.AddMember("what", Value(what(), alloc), alloc);
			detailsObjs.AddMember("currentSignCount", mCurrentSignCount, alloc);
			detailsObjs.AddMember("requiredSignCount", mRequiredSignCount, alloc);
			return std::move(detailsObjs);
		}

		// ******************************* Missing required sign *****************************************************
		TransactionValidationRequiredSignMissingException::TransactionValidationRequiredSignMissingException(const std::vector<MemoryBin*>& missingPublicKeys) noexcept
			: TransactionValidationException("missing required sign")
		{
			std::for_each(missingPublicKeys.begin(), missingPublicKeys.end(), [&](MemoryBin* pubkey) {
				mMissingPublicKeysHex.push_back(DataTypeConverter::binToHex(pubkey));
			});			
		}


		// ******************************** Invalid Pairing transaction **********************************************
		PairingTransactionNotMatchException::PairingTransactionNotMatchException(const char* what, const std::string* serializedTransaction, const std::string* serializedPairingTransaction) noexcept
			:TransactionValidationException(what) 
		{
			try {
				mTransaction = std::make_unique<model::gradido::GradidoTransaction>(serializedTransaction);
				mPairingTransaction = std::make_unique<model::gradido::GradidoTransaction>(serializedPairingTransaction);
			}
			catch (...) {
				printf("exception by creating transaction from serialized string\n");
			}
		}

		std::string PairingTransactionNotMatchException::getFullString() const noexcept
		{
			std::string resultString;
			auto transactionJson = mTransaction->toJson();
			auto pairedTransactionJson = mPairingTransaction->toJson();
			size_t resultSize = strlen(what()) + transactionJson.size() + pairedTransactionJson.size() + 4;
			resultString.reserve(resultSize);
			resultString = what();
			resultString += "\n";
			resultString += transactionJson + "\n";
			resultString += pairedTransactionJson;

			return resultString;
		}


		// *********************************** Address Already Exist **********************************************************
		AddressAlreadyExistException::AddressAlreadyExistException(
			const char* what,
			const std::string& addressHex,
			proto::gradido::RegisterAddress_AddressType addressType
		) noexcept
			: TransactionValidationException(what), mAddressHex(addressHex), mAddressType(addressType)
		{

		}

		std::string AddressAlreadyExistException::getFullString() const noexcept
		{
			std::string resultString;
			auto addressTypeString = model::gradido::RegisterAddress::getAddressStringFromType(mAddressType);
			size_t resultStringSize = strlen(what()) + addressTypeString.size() + mAddressHex.size() + 9 + 16 + 2;
			resultString.reserve(resultStringSize);
			resultString = what();
			resultString += ", address: " + mAddressHex;
			resultString += ", address type: " + addressTypeString;

			return resultString;

		}

		Value AddressAlreadyExistException::getDetails(Document::AllocatorType& alloc) const
		{
			Value detailsObjs(kObjectType);
			detailsObjs.AddMember("what", Value(what(), alloc), alloc);
			detailsObjs.AddMember("addressType", Value(model::gradido::RegisterAddress::getAddressStringFromType(mAddressType).data(), alloc), alloc);
			detailsObjs.AddMember("address", Value(mAddressHex.data(), alloc), alloc);
			return std::move(detailsObjs);
		}

		// *************************** Insufficient Balance Exception ************************************************
		InsufficientBalanceException::InsufficientBalanceException(const char* what, mpfr_ptr needed, mpfr_ptr exist) noexcept
			: TransactionValidationException(what)
		{
			TransactionBase::amountToString(&mNeeded, needed);
			TransactionBase::amountToString(&mExist, exist);
		}

		std::string InsufficientBalanceException::getFullString() const noexcept
		{
			std::string resultString;
			size_t resultSize = strlen(what()) + mNeeded.size() + mExist.size() + 2 + 10;
			resultString.reserve(resultSize);
			resultString = what();
			resultString += ", needed: " + mNeeded;
			resultString += ", exist: " + mExist;

			return resultString;
		}

		Value InsufficientBalanceException::getDetails(Document::AllocatorType& alloc) const
		{
			Value detailsObjs(kObjectType);
			detailsObjs.AddMember("what", Value(what(), alloc), alloc);
			detailsObjs.AddMember("needed", Value(mNeeded.data(), alloc), alloc);
			detailsObjs.AddMember("exist", Value(mExist.data(), alloc), alloc);
			return std::move(detailsObjs);
		}

		// **************************** Invalid Creation Exception *************************************
		InvalidCreationException::InvalidCreationException(
			const char* what,
			int targetMonth, int targetYear,
			const std::string& newCreationAmount,
			const std::string& alreadyCreatedBalance
		) noexcept
			: TransactionValidationException(what), mTargetMonth(targetMonth), mTargetYear(targetYear),
			mNewCreationAmount(newCreationAmount), mAlreadyCreatedBalance(alreadyCreatedBalance)
		{

		}

		std::string InvalidCreationException::getFullString() const noexcept
		{
			std::string result;
			auto targetDate = std::to_string(mTargetMonth) + " " + std::to_string(mTargetYear);
			size_t resultSize = strlen(what()) + 2 + targetDate.size() + 15;
			resultSize += mNewCreationAmount.size() + 21;
			resultSize += mAlreadyCreatedBalance.size() + 45;
			
			result.reserve(resultSize);
			result = what();
			result += ", target date: " + targetDate;
			result += ", try to create: " + mNewCreationAmount + " GDD";
			result += ", for this target already created: " + mAlreadyCreatedBalance + " GDD";

			return std::move(result);
		}


		Value InvalidCreationException::getDetails(Document::AllocatorType& alloc) const
		{
			Value detailsObjs(kObjectType);
			detailsObjs.AddMember("what", Value(what(), alloc), alloc);
			detailsObjs.AddMember("targetMonth", mTargetMonth, alloc);
			detailsObjs.AddMember("targetYear", mTargetYear, alloc);
			detailsObjs.AddMember("newCreationAmount", Value(mNewCreationAmount.data(), alloc), alloc);
			detailsObjs.AddMember("alreadyCreatedBalance", Value(mAlreadyCreatedBalance.data(), alloc), alloc);
			return std::move(detailsObjs);
		}

		// **************************** Wrong Address Type Exception ***********************************
		WrongAddressTypeException::WrongAddressTypeException(
			const char* what, 
			proto::gradido::RegisterAddress_AddressType type
		) noexcept
			: TransactionValidationException(what), mType(type)
		{

		}
		std::string WrongAddressTypeException::getFullString() const noexcept
		{
			std::string result;
			auto addressTypeName = proto::gradido::RegisterAddress_AddressType_Name(mType);
			size_t resultSize = strlen(what()) + addressTypeName.size() + 2 + 14;
			result.reserve(resultSize);
			result = what();
			result += "address type: " + addressTypeName;
			return result;
		}

		Value WrongAddressTypeException::getDetails(Document::AllocatorType& alloc) const
		{
			Value jsonDetails(kObjectType);
			jsonDetails.AddMember("what", Value(what(), alloc), alloc);

			auto addressTypeName = proto::gradido::RegisterAddress_AddressType_Name(mType);
			jsonDetails.AddMember("addressType", Value(addressTypeName.data(), alloc), alloc);
			return std::move(jsonDetails);
		}
	}
}