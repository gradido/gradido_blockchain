#include "gradido_blockchain/http/RequestExceptions.h"

#include <string>

using namespace rapidjson;

RequestException::RequestException(const char* what, const char* host, int port) noexcept
	: GradidoBlockchainException(what)
{
	mUri.setHost(host);
	mUri.setPort(port);
}

RequestException::RequestException(const char* what, const Poco::URI& uri) noexcept
	: GradidoBlockchainException(what), mUri(uri)
{

}

std::string RequestException::getFullString() const
{
	std::string result;
	std::string portString = std::to_string(mUri.getPort());
	size_t resultSize = strlen(what()) + portString.size() + mUri.getHost().size() + 16;
	result.reserve(resultSize);
	result = what();
	result += " by calling: " + mUri.getHost() + ":" + portString;
	return result;
}

// ************** Empty Response *******************************
RequestEmptyResponseException::RequestEmptyResponseException(const char* what, const char* host, int port) noexcept
	: RequestException(what, host, port)
{

}

RequestEmptyResponseException::RequestEmptyResponseException(const char* what, const Poco::URI& uri) noexcept
	: RequestException(what, uri)
{

}

// **************************** invalid json response ************************************
RequestResponseInvalidJsonException::RequestResponseInvalidJsonException(const char* what, const Poco::URI& uri, const std::string& rawText) noexcept
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


// ************************** error in json request ***************************************
RequestResponseErrorException::RequestResponseErrorException(const char* what, const Poco::URI& uri, const std::string& msg) noexcept
	: RequestException(what, uri), mErrorMessage(msg)
{

}

RequestResponseErrorException& RequestResponseErrorException::setDetails(const std::string& details)
{
	mErrorDetails = details;
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

// ******************* CakePHP Exception *****************************************
RequestResponseCakePHPException::RequestResponseCakePHPException(const char* what, const Poco::URI& uri, const std::string& msg) noexcept
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

// ********************** Poco Net Exception *******************************************
PocoNetException::PocoNetException(Poco::Exception& ex, const Poco::URI& uri, const char* query) noexcept
	: GradidoBlockchainException(ex.displayText().data()), mRequestUri(uri), mQuery(query)
{

}

std::string PocoNetException::getFullString() const
{
	std::string resultString;
	std::string host = mRequestUri.getHost();
	size_t resultSize = 2 + 17 + strlen(what()) + 17 + host.size() + mQuery.size();
	resultString.reserve(resultSize);

	resultString = "Poco Exception: ";
	resultString += what();
	resultString += " by calling url: " + host + mQuery;
	return resultString;
}
	
