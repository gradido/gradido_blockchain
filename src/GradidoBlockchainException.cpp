#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/GradidoUnit.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"
#include "rapidjson/error/en.h"

#include <string>
#include <sstream>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "magic_enum/magic_enum.hpp"

using namespace magic_enum;
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

Value RapidjsonParseErrorException::getDetails(Document::AllocatorType& alloc) const
{
	Value details(kObjectType);
	details.AddMember("what", Value(what(), alloc), alloc);
	details.AddMember("parseErrorCode", Value(GetParseError_En(mParseErrorCode), alloc), alloc);
	details.AddMember("parseErrorPosition", mParseErrorOffset, alloc);
	if (mRawText.size()) {
		details.AddMember("src", Value(mRawText.data(), alloc), alloc);
	}
	
	return std::move(details);
}

// ************************** Missing Member in Rapidjson Object ********************
RapidjsonMissingMemberException::RapidjsonMissingMemberException(const char* what, const char* fieldName, const char* fieldType) noexcept
	: GradidoBlockchainException(what), mFieldName(fieldName), mFieldType(fieldType)
{

}

std::string RapidjsonMissingMemberException::getFullString() const
{
	std::string result;
	result = what();
	if (mFieldName.size()) {
		result += ", field name: " + mFieldName;
	}
	if (mFieldType.size()) {
		result += ", field type: " + mFieldType;
	}
	return result;
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
GradidoUnknownEnumException::GradidoUnknownEnumException(const char* what, const char* enumName, const char* enumValue) noexcept
	: GradidoBlockchainException(what), mEnumName(enumName), mEnumValue(enumValue)
{

}

std::string GradidoUnknownEnumException::getFullString() const
{
	std::string resultString;
	size_t resultSize = strlen(what()) + mEnumName.size() + mEnumValue.size() + 2 + 13 + 9;
	resultString.reserve(resultSize);
	resultString = what();
	resultString += ", enum name: " + mEnumName;
	resultString += ", value: " + mEnumValue;
	return resultString;
}

// **************************** Invalid Base64 ****************************************
GradidoInvalidBase64Exception::GradidoInvalidBase64Exception(const char* what, const std::string& base64, size_t lastValidCharacterIndex) noexcept
	: GradidoBlockchainException(what), mBase64(base64), mLastValidCharacterIndex(lastValidCharacterIndex)
{
}

std::string GradidoInvalidBase64Exception::getFullString() const
{
	std::string resultString;	
	size_t resultSize = strlen(what()) + 8 + mBase64.size() + 14 + mLastValidCharacterIndex + 2;
	resultString.reserve(resultSize);
	resultString = what();
	resultString += ", with: " + mBase64 + ", valid part: ";
	if(mLastValidCharacterIndex < mBase64.size()) {
		resultString += mBase64.substr(0, mLastValidCharacterIndex);
	} else {
		resultString += mBase64;
	}
	
	return resultString;
}

// ********************************* Invalid Hex **********************************************
GradidoInvalidHexException::GradidoInvalidHexException(const char* what, const std::string& hex) noexcept
	: GradidoBlockchainException(what), mHex(hex)
{

}

std::string GradidoInvalidHexException::getFullString() const
{
	std::string resultString;
	size_t resultSize = strlen(what()) + mHex.size() + 10;
	resultString.reserve(resultSize);
	resultString = what();
	resultString += ", with: " + mHex;
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

// *************************** Insufficient Balance Exception ************************************************
InsufficientBalanceException::InsufficientBalanceException(const char* what, GradidoUnit needed, GradidoUnit exist) noexcept
	: GradidoBlockchainException(what), mNeeded(needed.toString()), mExist(exist.toString())
{
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


// ****************************** Null Pointer Exception **********************************************
GradidoNullPointerException::GradidoNullPointerException(const char* what, const char* typeName, const char* functionName) noexcept
	: GradidoBlockchainException(what), mTypeName(typeName), mFunctionName(functionName)
{

}

std::string GradidoNullPointerException::getFullString() const
{
	std::string resulString = what();
	resulString += ", type name: " + mTypeName + ", function name: " + mFunctionName;
	return resulString;
}

// ****************************** Invalid Size Exception **********************************************

std::string InvalidSizeException::getFullString() const
{
	std::string result = what();
	result += ", expected size: " + std::to_string(mExpectedSize);
	result += ", actual size: " + std::to_string(mActualSize);
	return result;
}

// **************************** Invalid Gradido Transaction Exception **********************************

std::string InvalidGradidoTransaction::getFullString() const 
{
	std::stringstream ss;
	ss << what();
	if(mRawData) {
		ss << ", raw data in hex for analysis with protoscope" << std::endl;
		ss << "xxd -r -ps <<< \"" << mRawData->convertToHex() << "\" | protoscope";
	}
	return ss.str();
}

// **************************** end date before start date exception **********************************
std::string EndDateBeforeStartDateException::getFullString() const 
{
	std::string result = what();
	result += ", start date: " + DataTypeConverter::timePointToString(mStartDate);
	result += ", end date: " + DataTypeConverter::timePointToString(mEndDate);
	return result;
}