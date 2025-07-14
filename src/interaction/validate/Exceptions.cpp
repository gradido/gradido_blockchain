#include "gradido_blockchain/data/GradidoTransaction.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/interaction/deserialize/Context.h"
#include "gradido_blockchain/interaction/toJson/Context.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

#include "loguru/loguru.hpp"
#include "magic_enum/magic_enum.hpp"

using namespace rapidjson;
using namespace magic_enum;

namespace gradido {
	namespace interaction {
		namespace validate {
			TransactionValidationException::TransactionValidationException(const char* what) noexcept
				: GradidoBlockchainException (what), mType(data::TransactionType::NONE)
			{

			}

			std::string TransactionValidationException::getFullString() const noexcept
			{
				return what();
			}

			TransactionValidationException& TransactionValidationException::setTransactionBody(const data::TransactionBody& transactionBody)
			{
				const auto& memos = transactionBody.getMemos();
				for (const auto& memo : memos) {
					if (memo.getKeyType() == data::MemoKeyType::PLAIN) {
						mTransactionMemo = memo.getMemo().copyAsString();
						break;
					}
				}
				mType = transactionBody.getTransactionType();
				return *this;
			}

			//************* Invalid Input *******************

			TransactionValidationInvalidInputException::TransactionValidationInvalidInputException(
				const char* what,
				const char* fieldName,
				const char* fieldType/* = nullptr*/,
				const char* expected/* = nullptr*/,
				const char* actual/* = nullptr*/
			) noexcept
				: TransactionValidationException(what), mFieldName(fieldName)
			{
				if (fieldType) {
					mFieldType = fieldType;
				}
				if(expected) {
					mExpected = expected;
				}
				if(actual) {
					mActual = actual;
				}
			}

			std::string TransactionValidationInvalidInputException::getFullString() const noexcept
			{
				std::string result;
				auto whatString = what();
				size_t stringSize = mFieldName.size() + mFieldType.size() + strlen(whatString) + 10;
				if(!mTransactionMemo.empty()) {
					stringSize += mTransactionMemo.size() + 17;
				}
				if(!mExpected.empty()) {
					stringSize += mExpected.size() + 10 + 2;
				}
				if(!mActual.empty()) {
					stringSize += mActual.size() + 8 + 2;
				}
				result.reserve(stringSize);
				result = whatString;
				if(!mTransactionMemo.empty()) {
					result += " with memo: " + mTransactionMemo + " and ";
				}
				result += " with " + mFieldName + ": " + mFieldType;
				if(!mExpected.empty()) {
					if(!result.empty()) result += ", ";
					result += "expected: " + mExpected;
				}
				if(!mActual.empty()) {
					if(!result.empty()) result += ", ";
					result += "actual: " + mActual;
				}
				return result;
			}
				Value TransactionValidationInvalidInputException::getDetails(Document::AllocatorType& alloc) const
			{
				Value detailsObjs(kObjectType);
				detailsObjs.AddMember("what", Value(what(), alloc), alloc);
				if(!mTransactionMemo.empty()) {
					detailsObjs.AddMember("memo", Value(mTransactionMemo.data(), alloc), alloc);
				}
				detailsObjs.AddMember("fieldName", Value(mFieldName.data(), alloc), alloc);
				if(!mFieldType.empty()) {
					detailsObjs.AddMember("fieldType", Value(mFieldType.data(), alloc), alloc);
				}
				if(!mExpected.empty()) {
					detailsObjs.AddMember("expected", Value(mExpected.data(), alloc), alloc);
				}
				if(!mActual.empty()) {
					detailsObjs.AddMember("actual", Value(mActual.data(), alloc), alloc);
				}
				
				return std::move(detailsObjs);
			}

			//************* Invalid Signature *******************

			TransactionValidationInvalidSignatureException::TransactionValidationInvalidSignatureException(
				const char* what, memory::ConstBlockPtr pubkey, memory::ConstBlockPtr signature, memory::ConstBlockPtr bodyBytes/* = ""*/
			) noexcept
				: TransactionValidationException(what), mPublicKey(pubkey), mSignature(signature), mBodyBytes(bodyBytes)
			{
			}

			TransactionValidationInvalidSignatureException::~TransactionValidationInvalidSignatureException()
			{
			}

			std::string TransactionValidationInvalidSignatureException::getFullString() const noexcept
			{
				std::string pubkeyHex = mPublicKey ? mPublicKey->convertToHex() : "";
				std::string signatureHex = mSignature ? mSignature->convertToHex() : "";
				std::string bodyBytesBase64 = mBodyBytes ? mBodyBytes->convertToBase64() : "";
				auto whatString = what();
				std::string result;

				size_t staticTextSize = 29;
				if (!bodyBytesBase64.empty()) {
					staticTextSize += 14;
				}
				result.reserve(pubkeyHex.size() + signatureHex.size() + bodyBytesBase64.size() + strlen(whatString) + staticTextSize);
				result = whatString;
				result += " with pubkey: " + pubkeyHex.substr(0, 64);
				result += ", signature: " + signatureHex.substr(0, 128);
				if (!bodyBytesBase64.empty()) {
					result += ", body bytes: " + bodyBytesBase64;
				}
				return result;
			}

			Value TransactionValidationInvalidSignatureException::getDetails(Document::AllocatorType& alloc) const
			{
				Value detailsObjs(kObjectType);
				detailsObjs.AddMember("what", Value(what(), alloc), alloc);
				if (mPublicKey) {
					detailsObjs.AddMember("pubkey", Value(mPublicKey->convertToHex().data(), alloc), alloc);
				}
				if (mSignature) {
					detailsObjs.AddMember("signature", Value(mSignature->convertToHex().data(), alloc), alloc);
				}
				if (mBodyBytes) {
					detailsObjs.AddMember("bodyBytes", Value(mBodyBytes->convertToHex().data(), alloc), alloc);
				}
				return std::move(detailsObjs);
			}

			// ************* Forbidden Sign *******************
			TransactionValidationForbiddenSignException::TransactionValidationForbiddenSignException(memory::ConstBlockPtr forbiddenPubkey) noexcept
				: TransactionValidationException("Forbidden Sign"), mForbiddenPubkey(forbiddenPubkey)
			{
			}
			TransactionValidationForbiddenSignException::~TransactionValidationForbiddenSignException()
			{

			}

			std::string TransactionValidationForbiddenSignException::getFullString() const noexcept
			{
				std::string forbiddenPubkeyHex = mForbiddenPubkey ? mForbiddenPubkey->convertToHex() : "";
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
					if(result.size()) {
						result += ", ";
					}
					result += "this forbidden pubkey was used for signing: " + forbiddenPubkeyHex;
				}
				return result;
			}

			Value TransactionValidationForbiddenSignException::getDetails(Document::AllocatorType& alloc) const
			{
				Value detailsObjs(kObjectType);
				detailsObjs.AddMember("what", Value(what(), alloc), alloc);
				if (mForbiddenPubkey) {
					detailsObjs.AddMember("forbiddenPubkey", Value(mForbiddenPubkey->convertToHex().data(), alloc), alloc);
				}
				detailsObjs.AddMember("memo", Value(mTransactionMemo.data(), alloc), alloc);
				return std::move(detailsObjs);
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
			TransactionValidationRequiredSignMissingException::TransactionValidationRequiredSignMissingException(const std::vector<memory::ConstBlockPtr>& missingPublicKeys) noexcept
				: TransactionValidationException("missing required sign")
			{
				for (auto pubkey : missingPublicKeys) {
					if (pubkey) {
						mMissingPublicKeysHex.push_back(pubkey->convertToHex());
					}
				}
			}

			// ******************************** Invalid Pairing transaction **********************************************
			PairingTransactionNotMatchException::PairingTransactionNotMatchException(
				const char* what,
				memory::ConstBlockPtr serializedTransaction,
				memory::ConstBlockPtr serializedPairingTransaction
			) noexcept
				:TransactionValidationException(what)
			{
				try {
					interaction::deserialize::Context deserializeTransaction(serializedTransaction, interaction::deserialize::Type::GRADIDO_TRANSACTION);
					deserializeTransaction.run();
					if (deserializeTransaction.isGradidoTransaction()) {
						mTransaction = deserializeTransaction.getGradidoTransaction();
					}
					interaction::deserialize::Context deserializePairingTransaction(serializedPairingTransaction, interaction::deserialize::Type::GRADIDO_TRANSACTION);
					deserializePairingTransaction.run();
					if (deserializePairingTransaction.isGradidoTransaction()) {
						mPairingTransaction = deserializePairingTransaction.getGradidoTransaction();
					}
				}
				catch (...) {
					LOG_F(WARNING, "exception by creating transaction from serialized string");
				}
			}

			std::string PairingTransactionNotMatchException::getFullString() const noexcept
			{
				std::string resultString;
				interaction::toJson::Context transactionToJson(*mTransaction.get());
				auto transactionJson = transactionToJson.run(true);
				interaction::toJson::Context paringTransactionToJson(*mPairingTransaction.get());
				auto pairedTransactionJson = paringTransactionToJson.run(true);
				size_t resultSize = strlen(what()) + transactionJson.size() + pairedTransactionJson.size() + 4;
				resultString.reserve(resultSize);
				resultString = what();
				resultString += "\n";
				resultString += transactionJson;
				resultString += "\n";
				resultString += pairedTransactionJson;

				return resultString;
			}


			// *********************************** Address Already Exist **********************************************************
			AddressAlreadyExistException::AddressAlreadyExistException(const char* what, const std::string& addressHex, data::AddressType addressType) noexcept
				: TransactionValidationException(what), mAddressHex(addressHex), mAddressType(addressType)
			{

			}

			std::string AddressAlreadyExistException::getFullString() const noexcept
			{
				std::string resultString;
				auto addressTypeString = enum_name(mAddressType);
				size_t resultStringSize = strlen(what()) + addressTypeString.size() + mAddressHex.size() + 9 + 16 + 2;
				resultString.reserve(resultStringSize);
				resultString = what();
				resultString += ", address: " + mAddressHex;
				resultString += ", address type: ";
				resultString += addressTypeString;

				return resultString;

			}

			Value AddressAlreadyExistException::getDetails(Document::AllocatorType& alloc) const
			{
				Value detailsObjs(kObjectType);
				detailsObjs.AddMember("what", Value(what(), alloc), alloc);
				detailsObjs.AddMember("addressType", Value(enum_name(mAddressType).data(), alloc), alloc);
				detailsObjs.AddMember("address", Value(mAddressHex.data(), alloc), alloc);
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
			WrongAddressTypeException::WrongAddressTypeException(const char* what, data::AddressType type, memory::ConstBlockPtr pubkey) noexcept
				: TransactionValidationException(what), mType(type), mPublicKey(pubkey)
			{

			}
			std::string WrongAddressTypeException::getFullString() const noexcept
			{
				std::string result;
				auto addressTypeName = enum_name(mType);
				std::string pubkeyHex;
				if (mPublicKey) {
					pubkeyHex = mPublicKey->convertToHex();
				}
				size_t resultSize = strlen(what()) + addressTypeName.size() + 2 + 14 + 10 + pubkeyHex.size();
				result.reserve(resultSize);
				result = what();
				result += ", address type: ";
				result += addressTypeName;
				result += ", pubkey: " + pubkeyHex;

				return result;
			}

			Value WrongAddressTypeException::getDetails(Document::AllocatorType& alloc) const
			{
				Value jsonDetails(kObjectType);
				jsonDetails.AddMember("what", Value(what(), alloc), alloc);

				auto addressTypeName = enum_name(mType);
				jsonDetails.AddMember("addressType", Value(addressTypeName.data(), alloc), alloc);
				return std::move(jsonDetails);
			}
		}
	}
}

