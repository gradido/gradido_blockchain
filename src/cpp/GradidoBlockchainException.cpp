#include "GradidoBlockchainException.h"

#include "rapidjson/error/en.h"

#include <string>

using namespace rapidjson;

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