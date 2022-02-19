#include "gradido_blockchain/GradidoBlockchainException.h"

#include "rapidjson/error/en.h"

#include <string>

using namespace rapidjson;

GradidoBlockchainTransactionNotFoundException::GradidoBlockchainTransactionNotFoundException(const char* what) noexcept
	: GradidoBlockchainException(what), mTransactionId(0)
{

}

std::string GradidoBlockchainTransactionNotFoundException::getFullString() const
{
	std::string result;
	std::string transactionIdString;
	size_t resultSize = strlen(what()) + 2;
	if (mTransactionId) {
		transactionIdString = std::to_string(mTransactionId);
		resultSize += transactionIdString.size() + 17;
	}
	result.reserve(resultSize);
	result = what();
	result += ", transactionId: " + transactionIdString;
	return result;
}

// ################################ RapidJson Parse Error Exception ###############################################################

RapidjsonParseErrorException::RapidjsonParseErrorException(const char* what, ParseErrorCode parseErrorCode, size_t parseErrorOffset) noexcept
	: GradidoBlockchainException(what), mParseErrorCode(parseErrorCode), mParseErrorOffset(parseErrorOffset)
{

}

RapidjsonParseErrorException& RapidjsonParseErrorException::setRawText(const std::string& rawText)
{
	mRawText = rawText;
	return *this;
}
std::string RapidjsonParseErrorException::getFullString() const
{
	std::string resultString;
	auto parseErrorCodeString = GetParseError_En(mParseErrorCode);
	std::string parseErrorOffsetString = std::to_string(mParseErrorOffset);

	size_t resultSize = strlen(what()) + strlen(parseErrorCodeString) + 15 + parseErrorOffsetString.size() + 14 + 2; 
	if (mRawText.size()) {
		resultSize += mRawText.size() + 7;
	}
	resultString = what();
	resultString += ", parse error: ";
	resultString += parseErrorCodeString;
	resultString += " on position: " + parseErrorOffsetString;
	if (mRawText.size()) {
		resultString += ", src: " + mRawText;
	}
	return resultString;
}

// *************************** Invalid Enum Exception *****************************
GradidoInvalidEnumException::GradidoInvalidEnumException(const char* what, const std::string& enumString) noexcept
	: GradidoBlockchainException(what), mEnumString(enumString)
{

}

std::string GradidoInvalidEnumException::getFullString() const
{
	std::string resultString;
	size_t resultSize = strlen(what()) + mEnumString.size() + 10;
	resultString.reserve(resultSize);
	resultString = what();
	resultString += ", with: " + mEnumString;
	return resultString;
}

// **************************** Invalid Base64 ****************************************
GradidoInvalidBase64Exception::GradidoInvalidBase64Exception(const char* what, const std::string& base64, int lastValidCharacter) noexcept
	: GradidoBlockchainException(what), mBase64(base64), mLastValidCharacter(lastValidCharacter)
{

}

std::string GradidoInvalidBase64Exception::getFullString() const
{
	std::string resultString;
	std::string lastValidCharacterString = std::to_string(mLastValidCharacter);
	size_t resultSize = strlen(what()) + mBase64.size() + 10 + lastValidCharacterString.size() + 19;
	resultString.reserve(resultSize);
	resultString = what();
	resultString += ", with: " + mBase64 + ", last valid char: " + lastValidCharacterString;
	return resultString;
}
