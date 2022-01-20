#include "IotaRequest.h"
#include "lib/Profiler.h"

#include <algorithm>
#include <cctype>

#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "ServerConfig.h"

using namespace rapidjson;


IotaRequest::IotaRequest(const std::string& serverHost, int serverPort, const std::string& urlPath)
	: JsonRequest(serverHost, serverPort)
{
	mRequestUri.setPath(urlPath);
}

IotaRequest::~IotaRequest()
{

}

std::vector<std::string> IotaRequest::getTips(NotificationList* errorReciver)
{
	static const char* functionName = "IotaRequest::getTips";
	std::string fullPath = mRequestUri.getPath() + "tips";
	auto responseString = GET(fullPath.data(), "HTTP/1.1");
	if (responseString == "client session zero") {
		errorReciver->getErrors(this);
		return {};
	}
	
	auto json = parseResponse(responseString, errorReciver);

	if (!json.HasMember("data")) {
		errorReciver->addError(new Error(functionName, "data member missing"));
		return {};
	}
	Value& data = json["data"];
	if (!data.HasMember("tipMessageIds") || !data["tipMessageIds"].IsArray()) {
		errorReciver->addError(new Error(functionName, "tipMessageIds member missing"));
		return {};
	}

	std::vector<std::string> parentIds;
	auto tips = data["tipMessageIds"].GetArray();
	parentIds.reserve(tips.Size());
	for (auto it = tips.Begin(); it != tips.End(); it++) {
		parentIds.push_back(it->GetString());
	}
	
	return parentIds;
}



std::string IotaRequest::sendMessage(const std::string& indexHex, const std::string& messageHex, NotificationList* errorReciver)
{
	static const char* functionName = "IotaRequest::sendMessage";
	
	auto tips = getTips(errorReciver);
	if (!tips.size()) {
		errorReciver->addError(new Error(functionName, "no get tips"));
		return "";
	}

	Document requestJson(kObjectType);
	auto alloc = requestJson.GetAllocator();
	requestJson.AddMember("networkId", "", alloc);

	Value parentMessageIds(kArrayType);
//	int i = 0;
	for (auto it = tips.begin(); it != tips.end(); it++) {
		//if (i > 1) break;
		parentMessageIds.PushBack(Value(it->data(), alloc), alloc);
		//i++;
	}
	requestJson.AddMember("parentMessageIds", parentMessageIds, alloc);
	Value payload(kObjectType);
	payload.AddMember("type", 2, alloc);
	std::string upperIndex;
	for (int i = 0; i < indexHex.size(); i++) {
		upperIndex.push_back((std::toupper(indexHex.data()[i])));
	}
	std::string upperMessage;
	for (int i = 0; i < messageHex.size(); i++) {
		upperMessage.push_back((std::toupper(messageHex.data()[i])));
	}

	payload.AddMember("index", Value(indexHex.data(), alloc), alloc);
	payload.AddMember("data", Value(messageHex.data(), alloc), alloc);

	requestJson.AddMember("payload", payload, alloc);
	// TODO: calculate nonce
	requestJson.AddMember("nonce", "", alloc);

	auto fullPath = mRequestUri.getPath() + "messages";
	auto responseString = POST(fullPath.data(), requestJson, "HTTP/1.1");
	
	auto json = parseResponse(responseString, errorReciver);
	if (!json.HasMember("data")) {
		errorReciver->addError(new Error(functionName, "data member in response missing"));
		return "";
	}
	Value& data = json["data"];
	if (!data.HasMember("messageId") || !data["messageId"].IsString()) {
		errorReciver->addError(new Error(functionName, "messageId in response is missing or not a string"));
		return "";
	}
	return data["messageId"].GetString();	

}
