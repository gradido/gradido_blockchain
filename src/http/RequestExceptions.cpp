#include "gradido_blockchain/http/RequestExceptions.h"

#include <string>

using namespace rapidjson;

RequestException::RequestException(const char* what, const char* host, int port) noexcept
	: GradidoBlockchainException(what)
{
	mUri = std::string(host);
	mUri += ':' + std::to_string(port);
}

RequestException::RequestException(const char* what, const std::string& uri) noexcept
	: GradidoBlockchainException(what), mUri(uri)
{

}

std::string RequestException::getFullString() const
{
	std::string result;
	size_t resultSize = strlen(what()) + mUri.size() + 15;
	result.reserve(resultSize);
	result = what();
	result += " by calling: " + mUri;
	return result;
}

// ************** Empty Response *******************************
RequestEmptyResponseException::RequestEmptyResponseException(const char* what, const char* host, int port) noexcept
	: RequestException(what, host, port)
{

}

RequestEmptyResponseException::RequestEmptyResponseException(const char* what, const std::string& uri) noexcept
	: RequestException(what, uri)
{

}

// **************************** invalid json response ************************************
RequestResponseInvalidJsonException::RequestResponseInvalidJsonException(const char* what, const std::string& uri, const std::string& rawText) noexcept
	: RequestException(what, uri), mRawText(rawText)
{

}

bool RequestResponseInvalidJsonException::printToFile(const char* prefix, const char* ending/* = ".txt"*/) const
{
	std::string fileName = prefix;
	fileName += what();
	fileName += ending;
		
	FILE* f = fopen(fileName.data(), "wt");
	if (f) {
		fwrite(mRawText.data(), 1, mRawText.size(), f);
		fclose(f);
		return true;
	}	
	return false;
}

bool RequestResponseInvalidJsonException::containRawHtmlClosingTag() const
{
	return (mRawText.find("</html>") != mRawText.npos);
}

// ************************** error in json request ***************************************
RequestResponseErrorException::RequestResponseErrorException(const char* what, const std::string& uri, const std::string& msg) noexcept
	: RequestException(what, uri), mErrorMessage(msg)
{

}

RequestResponseErrorException& RequestResponseErrorException::setDetails(const std::string& details)
{
	mErrorDetails = details;
	return *this;
}

RequestResponseErrorException& RequestResponseErrorException::setDetails(const rapidjson::Value& details)
{
	if (details.IsString()) {
		if (details.GetStringLength()) {
			mErrorDetails = std::string(details.GetString(), details.GetStringLength());
		}
	}
	else if (details.IsArray()) {
		mErrorDetails = "";
		for (auto it = details.Begin(); it != details.End(); it++) {
			mErrorDetails += it->GetString();
			mErrorDetails += "\n";
		}
	}
	else if (details.IsObject()) {
		mErrorDetails = "";
		for (auto it = details.MemberBegin(); it != details.MemberEnd(); it++) {
			mErrorDetails += it->name.GetString();
			mErrorDetails += ": ";
			if (it->value.IsString()) {
				mErrorDetails += it->value.GetString();
			}
			else if (it->value.IsInt()) {
				mErrorDetails += std::to_string(it->value.GetInt());
			}
			else {
				mErrorDetails += "<not a string or int>";
			}
			mErrorDetails += "\n";
		}
	}
	return *this;
}

std::string RequestResponseErrorException::getFullString() const
{
	auto resultString = RequestException::getFullString();
	size_t resultSize = resultString.size() + mErrorMessage.size() + 7 + 2;
	if (mErrorDetails.size()) {
		resultSize += mErrorDetails.size() + 12;
	}

	resultString.reserve(resultSize);
	resultString += ", msg: " + mErrorMessage;
	if (mErrorDetails.size()) {
		resultString += ", details: " + mErrorDetails;
	}
	return resultString;
}

Value RequestResponseErrorException::getDetails(Document::AllocatorType& alloc) const
{
	Value result(kObjectType);
	result.AddMember("what", Value(what(), alloc), alloc);
	result.AddMember("uri", Value(mUri.data(), alloc), alloc);
	result.AddMember("msg", Value(mErrorMessage.data(), alloc), alloc);
	if (mErrorDetails.size()) {
		result.AddMember("details", Value(mErrorDetails.data(), alloc), alloc);
	}
	return result;
}

// **************** HttplibRequestException **************************************
HttplibRequestException::HttplibRequestException(const char* what, const std::string& uri, int status, const char* error) noexcept
	: RequestException(what, uri), mStatus(status), mError(error)
{
	
}

std::string HttplibRequestException::getFullString() const
{
	auto resultString = RequestException::getFullString();
	resultString += ", status: " + std::to_string(mStatus);
	resultString += ", error enum: " + mError;
	return resultString;
}

// ******************* CakePHP Exception *****************************************
RequestResponseCakePHPException::RequestResponseCakePHPException(const char* what, const std::string& uri, const std::string& msg) noexcept
	: RequestResponseErrorException(what, uri, msg)
{

}

RequestResponseErrorException& RequestResponseCakePHPException::setDetails(const Document& responseJson)
{
	mErrorDetails = "";
	
	std::string fields[] = { "url", "code", "file", "line" };
	for (int i = 0; i < 4; i++) {
		auto field = fields[i];
		std::string field_name = field + ": ";
		std::string value;
		Value::ConstMemberIterator itr = responseJson.FindMember(field.data());
		if (itr != responseJson.MemberEnd()) {
			if (itr->value.IsString()) {
				value = itr->value.GetString();
			}
			else if (itr->value.IsInt()) {
				value = std::to_string(itr->value.GetInt());
			}
			mErrorDetails += field_name + value + ", ";
		}
	}
	return *this;
}

// ****************** Handle Request Exception ************************************
HandleRequestException::HandleRequestException(const char* what) noexcept
	: GradidoBlockchainException(what)
{

}

std::string HandleRequestException::getFullString() const
{
	return what();
}


// *********************** JSON RPC Request Exceptions *************************************
JsonRPCException::JsonRPCException(const char* what, JsonRPCErrorCodes errorCode) noexcept
	: GradidoBlockchainException(what), mErrorCode(errorCode)
{

}

std::string JsonRPCException::getFullString() const
{
	std::string result(what());
	result += ", errorCode: " + std::to_string(mErrorCode);
	return result;
}

