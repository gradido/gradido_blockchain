#include "TransactionValidationExceptions.h"

#include <sstream>

#include "lib/DataTypeConverter.h"

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

		//************* Invalid Signature *******************

		TransactionValidationInvalidSignatureException::TransactionValidationInvalidSignatureException(
			const char* what, MemoryBin* pubkey, MemoryBin* signature, MemoryBin* bodyBytes/* = nullptr*/
		) noexcept
			: TransactionValidationException(what), mPubkey(pubkey), mSignature(signature), mBodyBytes(bodyBytes)
		{

		}

		TransactionValidationInvalidSignatureException::~TransactionValidationInvalidSignatureException()
		{
			auto mm = MemoryManager::getInstance();
			mm->releaseMemory(mBodyBytes);
			mm->releaseMemory(mPubkey);
			mm->releaseMemory(mSignature);
		}

		std::string TransactionValidationInvalidSignatureException::getFullString() const noexcept
		{
			std::string pubkeyHex;
			std::string signatureHex;
			std::string bodyBytesBase64;
			auto whatString = what();
			std::string result;
			if (mPubkey) {
				try {
					pubkeyHex = DataTypeConverter::binToHex(mPubkey);
				}
				catch (...) {}
			}
			if (mSignature) {
				try {
					signatureHex = DataTypeConverter::binToHex(mSignature);
				}
				catch (...) {}
			}
			if (mBodyBytes) {
				try {
					bodyBytesBase64 = DataTypeConverter::binToBase64(mBodyBytes);
				} catch(...) {}
			}
			size_t staticTextSize = 29;
			if (mBodyBytes) {
				staticTextSize += 14;
			}
			result.reserve(pubkeyHex.size() + signatureHex.size() + bodyBytesBase64.size() + strlen(whatString) + staticTextSize);
			result = whatString;
			result += " with pubkey: " + pubkeyHex;
			result += ", signature: " + signatureHex;
			if (mBodyBytes) {
				result += ", body bytes: " + bodyBytesBase64;
			}
			return result;
		}

		// ************* Forbidden Sign *******************
		TransactionValidationForbiddenSignException::TransactionValidationForbiddenSignException(MemoryBin* forbiddenPubkey, const std::string& memo)
			: TransactionValidationException("Forbidden Sign")
		{
			mForbiddenPubkey = forbiddenPubkey;
			setMemo(memo);
		}
		TransactionValidationForbiddenSignException::~TransactionValidationForbiddenSignException()
		{
			if (mForbiddenPubkey) {
				MemoryManager::getInstance()->releaseMemory(mForbiddenPubkey);
			}
		}

		std::string TransactionValidationForbiddenSignException::getFullString() const noexcept
		{
			std::string forbiddenPubkeyHex;
			if (mForbiddenPubkey) {
				try {
					forbiddenPubkeyHex = DataTypeConverter::binToHex(mForbiddenPubkey);
				}
				catch (...) {}
			}
			size_t resultSize = mTransactionMemo.size() + 25;
			if (forbiddenPubkeyHex.size()) {
				resultSize += forbiddenPubkeyHex.size() + 47;
			}
			std::string result;
			result.reserve(resultSize);
			result = "transaction with memo: " + mTransactionMemo;
			if (forbiddenPubkeyHex.size()) {
				result += ", this forbidden pubkey was used for signing: " + forbiddenPubkeyHex;
			}
			return result;
		}
	}
}