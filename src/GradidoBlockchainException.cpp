#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/model/protobufWrapper/TransactionBase.h"
#include "rapidjson/error/en.h"

#include <string>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"

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

// ################################ Transaction Already Exist Exception ###########################################################
GradidoBlockchainTransactionAlreadyExistException::GradidoBlockchainTransactionAlreadyExistException(const char* what) noexcept
	: GradidoBlockchainException(what), mTransactionId(0)
{

}

std::string GradidoBlockchainTransactionAlreadyExistException::getFullString() const
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

std::string RapidjsonParseErrorException::getDetails() const
{
	Document details(kObjectType);
	auto alloc = details.GetAllocator();
	details.AddMember("what", Value(what(), alloc), alloc);
	details.AddMember("parseErrorCode", Value(GetParseError_En(mParseErrorCode), alloc), alloc);
	details.AddMember("parseErrorPosition", mParseErrorOffset, alloc);
	if (mRawText.size()) {
		details.AddMember("src", Value(mRawText.data(), alloc), alloc);
	}
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	details.Accept(writer);

	return std::move(std::string(buffer.GetString()));
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

// ***************** it an enuam value is unknown by code ***************************
GradidoUnknownEnumException::GradidoUnknownEnumException(const char* what, const char* enumName, int value) noexcept
	: GradidoBlockchainException(what), mEnumName(enumName), mValue(value)
{

}

std::string GradidoUnknownEnumException::getFullString() const
{
	std::string resultString;
	auto valueString = std::to_string(mValue);
	size_t resultSize = strlen(what()) + mEnumName.size() + valueString.size() + 2 + 13 + 9;
	resultString.reserve(resultSize);
	resultString = what();
	resultString += ", enum name: " + mEnumName;
	resultString += ", value: " + valueString;
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

// ######################### Blockchain Order Exception ####################################
BlockchainOrderException::BlockchainOrderException(const char* what) noexcept
	: GradidoBlockchainException(what)
{

}

std::string BlockchainOrderException::getFullString() const
{
	return what();
}

// *************************** Invalid Transaction Type on Blockchain
InvalidTransactionTypeOnBlockchain::InvalidTransactionTypeOnBlockchain(const char* what, model::gradido::TransactionType type) noexcept
	: GradidoBlockchainException(what), mTransactionType(type)
{

}

std::string InvalidTransactionTypeOnBlockchain::getFullString() const
{
	auto transactionTypeString = model::gradido::TransactionBase::getTransactionTypeString(mTransactionType);
	std::string resultString;
	resultString.reserve(strlen(what()) + strlen(transactionTypeString) + 2 + 20);
	resultString = what();
	resultString += ", transaction type: ";
	resultString += transactionTypeString;
	return resultString;
}