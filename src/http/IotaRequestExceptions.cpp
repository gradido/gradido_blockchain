#include "gradido_blockchain/http/IotaRequestExceptions.h"

#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;

IotaRequestException::IotaRequestException(const char* what, const std::string& url) noexcept
	: GradidoBlockchainException(what), mUrl(url)
{

}

std::string IotaRequestException::getFullString() const
{
	std::string resultString;
	size_t resultSize = strlen(what()) + mUrl.size() + 12;

	resultString.reserve(resultSize);
	resultString = what();
	resultString += ", called: " + mUrl;
	return resultString;
}

// *************************  Iota Post Request ************************

IotaPostRequestException::IotaPostRequestException(const char* what, const std::string& url) noexcept
	: IotaRequestException(what, url)
{

}

IotaPostRequestException& IotaPostRequestException::setRequestJson(const rapidjson::Document& requestJson)
{
	StringBuffer buffer;
	PrettyWriter<StringBuffer> writer(buffer);
	requestJson.Accept(writer);
	mRequestJsonPrettyString = std::string(buffer.GetString(), buffer.GetSize());
	return *this;
}

std::string IotaPostRequestException::getFullString() const
{
	auto resultString = IotaRequestException::getFullString();
	size_t resultSize = resultString.size();
	resultSize += mRequestJsonPrettyString.size() + 11;
	resultString.reserve(resultSize);
	resultString += ", json: \n" + mRequestJsonPrettyString;
	return resultString;
}