
#include "gradido_blockchain/http/JsonRequest.h"

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "gradido_blockchain/http/RequestExceptions.h"

using namespace rapidjson;

JsonRequest::JsonRequest(const std::string& serverHost, int serverPort)
	: HttpRequest(serverHost, serverPort), mJsonDocument(kObjectType)
{

}

JsonRequest::JsonRequest(const std::string& requestUri)
	: HttpRequest(requestUri), mJsonDocument(kObjectType)
{

}

JsonRequest::~JsonRequest()
{

}

Document JsonRequest::postRequest(const char* path, Value& payload)
{
	auto alloc = mJsonDocument.GetAllocator();

	if (payload.IsObject()) {
		for (auto it = payload.MemberBegin(); it != payload.MemberEnd(); it++) {
			mJsonDocument.AddMember(it->name, it->value, alloc);
		}
	}
	auto responseString = POST(path, mJsonDocument);
	auto responseJson = parseResponse(responseString);
	
	return responseJson;
}

Document JsonRequest::getRequest(const char* path)
{
	auto responseString = GET({}, path);
	auto responseJson = parseResponse(responseString);
	return responseJson;
}

rapidjson::Document JsonRequest::postRequest(rapidjson::Value& payload)
{
	auto alloc = mJsonDocument.GetAllocator();

	if (payload.IsObject()) {
		for (auto it = payload.MemberBegin(); it != payload.MemberEnd(); it++) {
			mJsonDocument.AddMember(it->name, it->value, alloc);
		}
	}
	auto responseString = POST(nullptr, mJsonDocument);
	auto responseJson = parseResponse(responseString);

	return responseJson;
}


std::string JsonRequest::POST(const char* path, const rapidjson::Document& payload)
{
	addHeader("Content-Type", "application/json");
	addHeader("Accept", "*/*");
	
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	payload.Accept(writer);
	std::string body(buffer.GetString(), buffer.GetSize());
	return HttpRequest::POST(body);
}

Document JsonRequest::parseResponse(std::string responseString)
{
	Document result;
	// extract parameter from request
	result.Parse(responseString.data());

	if (result.HasParseError()) {
		throw RapidjsonParseErrorException("error parsing request answer", result.GetParseError(), result.GetErrorOffset())
			.setRawText(responseString);
	}
	return result;
}