#include "gradido_blockchain/http/IotaRequest.h"
#include "gradido_blockchain/http/IotaRequestExceptions.h"

#include "gradido_blockchain/lib/Profiler.h"

#include <algorithm>
#include <cctype>

#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"


using namespace rapidjson;


IotaRequest::IotaRequest(const std::string& serverHost, int serverPort, const std::string& urlPath)
	: JsonRequest(serverHost, serverPort)
{
	mRequestUri.setPath(urlPath);
}

IotaRequest::~IotaRequest()
{

}

std::vector<std::string> IotaRequest::getTips()
{
	std::string fullPath = mRequestUri.getPath() + "tips";
	auto responseString = GET(fullPath.data(), "HTTP/1.1");
		
	auto json = parseResponse(responseString);

	if (!json.HasMember("data")) {
		throw IotaRequestException("data member missing", fullPath);
	}
	Value& data = json["data"];
	if (!data.HasMember("tipMessageIds") || !data["tipMessageIds"].IsArray()) {
		throw IotaRequestException("tipMessageIds member missing", fullPath);
	}

	std::vector<std::string> parentIds;
	auto tips = data["tipMessageIds"].GetArray();
	parentIds.reserve(tips.Size());
	for (auto it = tips.Begin(); it != tips.End(); it++) {
		parentIds.push_back(it->GetString());
	}
	
	return parentIds;
}



std::string IotaRequest::sendMessage(const std::string& indexHex, const std::string& messageHex)
{	
	auto tips = getTips();
	if (!tips.size()) {
		throw IotaRequestException("no tips", mRequestUri.getPath() + "tips");
	}

	Document requestJson(kObjectType);
	auto alloc = requestJson.GetAllocator();
	requestJson.AddMember("networkId", "", alloc);

	Value parentMessageIds(kArrayType);

	for (auto it = tips.begin(); it != tips.end(); it++) {
		parentMessageIds.PushBack(Value(it->data(), alloc), alloc);
		//if (parentMessageIds.Size() > 2) break;
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
	
	auto json = parseResponse(responseString);
	if (!json.HasMember("data")) {
		throw IotaPostRequestException("data member in response missing", fullPath)
			.setRequestJson(requestJson);
	}
	Value& data = json["data"];
	if (!data.HasMember("messageId") || !data["messageId"].IsString()) {
		throw IotaPostRequestException("messageId in response is missing or not a string", fullPath)
			.setRequestJson(requestJson);
	}
	return data["messageId"].GetString();
}
