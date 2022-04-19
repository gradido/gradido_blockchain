#include "gradido_blockchain/http/IotaRequest.h"
#include "gradido_blockchain/http/IotaRequestExceptions.h"
#include "gradido_blockchain/http/RequestExceptions.h"

#include "gradido_blockchain/lib/Profiler.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

#include <algorithm>
#include <cctype>

#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Util/ServerApplication.h"

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/pointer.h"



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

rapidjson::Document IotaRequest::getMessageJson(const std::string& messageIdHex)
{
	// GET /api/v1/messages/{messageId} 
	
	std::string fullPath;
	fullPath.reserve(mRequestUri.getPath().size() + 9 + messageIdHex.size());
	fullPath = mRequestUri.getPath() + "messages/";
	fullPath += messageIdHex;
	return parseResponse(GET(fullPath.data(), "HTTP/1.1"));
}

std::pair<std::unique_ptr<std::string>, std::unique_ptr<std::string>> IotaRequest::getIndexiationMessageDataIndex(const std::string& messageIdHex)
{
	auto json = getMessageJson(messageIdHex);
	if (!json.IsObject() || json.FindMember("data") == json.MemberEnd()) {
		throw IotaRequestException("error calling messages from iota, no valid json or no data member found", mRequestUri.getPath() + "messages/" + messageIdHex);
	}
	auto payload = Pointer("/data/payload").Get(json);
	if (!payload->HasMember("data") || !payload->HasMember("index")) {
		throw IotaRequestException("json field data.payload.data or data.payload.index missing", mRequestUri.getPath() + "messages/" + messageIdHex);
	}
	return { DataTypeConverter::hexToBinString((*payload)["data"].GetString()), DataTypeConverter::hexToBinString((*payload)["index"].GetString()) };
}

uint32_t IotaRequest::getMessageMilestoneId(const std::string& messageIdHex)
{
	// GET /api/v1/messages/{messageId}/metadata
	std::string fullPath;
	fullPath.reserve(mRequestUri.getPath().size() + 18 + messageIdHex.size());
	fullPath = mRequestUri.getPath() + "messages/" + messageIdHex + "/metadata";

	auto json = parseResponse(GET(fullPath.data(), "HTTP/1.1"));

	if (!json.IsObject() || json.FindMember("data") == json.MemberEnd()) {
		throw IotaRequestException("error calling for message metadata", fullPath);
	}
	Value& data = json["data"];
	auto referencedByMilestoneIt = data.FindMember("referencedByMilestoneIndex");
	if (referencedByMilestoneIt != data.MemberEnd()) {
		return referencedByMilestoneIt->value.GetUint();
	}
	return 0;
}

std::vector<MemoryBin*> IotaRequest::findByIndex(const std::string& index)
{
	// GET /api/v1/messages?index=4752414449444f2e7079746861676f726173
	std::string fullPath;
	auto indexHex = DataTypeConverter::binToHex(index);
	fullPath.reserve(mRequestUri.getPath().size() + 18 + indexHex.size());
	fullPath = mRequestUri.getPath() + "messages?index=" + indexHex;

	std::vector<MemoryBin*> result;

	auto json = parseResponse(GET(fullPath.data(), "HTTP/1.1"));
	if (!json.IsObject() || !json.HasMember("data")) {
		throw IotaRequestException("findByIndex failed or data not set", fullPath);
	}

	Value& data = json["data"];
	if (!data.HasMember("messageIds") || !data["messageIds"].IsArray()) {
		throw IotaPostRequestException("messageIds not found or not an array", fullPath);
	}
	auto messageIds = data["messageIds"].GetArray();
	for (auto it = messageIds.Begin(); it != messageIds.End(); ++it) {
		result.push_back(DataTypeConverter::hexToBin(it->GetString()));
	}

	return result;
}

MemoryBin* IotaRequest::getMilestoneByIndex(int32_t milestoneIndex)
{
	// GET api/v1/milestones/909039
	std::string fullPath;
	auto milestoneIndexString = std::to_string(milestoneIndex);
	fullPath.reserve(mRequestUri.getPath().size() + 11 + milestoneIndexString.size());
	fullPath = mRequestUri.getPath() + "milestones/" + milestoneIndexString;

	auto json = parseResponse(GET(fullPath.data(), "HTTP/1.1"));
	if (!json.IsObject() || !json.HasMember("data")) {
		throw IotaRequestException("getMilestoneByIndex failed or data not set", fullPath);
	}
	Value& data = json["data"];
	if (!data.HasMember("messageId") || !data["messageId"].IsString()) {
		throw IotaRequestException("messageId field not set or not string", fullPath);
	}
	return DataTypeConverter::hexToBin(data["messageId"].GetString());
}

uint64_t IotaRequest::getMilestoneTimestamp(int32_t milestoneIndex)
{
	// GET api/v1/milestones/909039
	std::string fullPath;
	auto milestoneIndexString = std::to_string(milestoneIndex);
	fullPath.reserve(mRequestUri.getPath().size() + 11 + milestoneIndexString.size());
	fullPath = mRequestUri.getPath() + "milestones/" + milestoneIndexString;

	auto json = parseResponse(GET(fullPath.data(), "HTTP/1.1"));
	if (!json.IsObject() || !json.HasMember("data")) {
		throw IotaRequestException("getMilestoneByIndex failed or data not set", fullPath);
	}
	Value& data = json["data"];
	if (!data.HasMember("timestamp") || !data["timestamp"].IsUint64()) {
		throw IotaRequestException("timestamp field not set or not uint64", fullPath);
	}
	return data["timestamp"].GetUint64();
}

iota::NodeInfo IotaRequest::getNodeInfo()
{
	iota::NodeInfo result;
	auto fullPath = mRequestUri.getPath() + "info";
	auto json = parseResponse(GET(fullPath.data(), "HTTP/1.1"));
	if (!json.IsObject()) return result;
	
	if (!json.HasMember("data")) {
		throw IotaRequestException("data member in response missing", fullPath);
	}
	Value& data = json["data"];
	if (!data.HasMember("version") || !data["version"].IsString()) {
		throw IotaRequestException("version in response missing or not a string", fullPath);
	}
	result.version = data["version"].GetString(); // testet with 1.0.5
	if (!data.HasMember("isHealthy") || !data["isHealthy"].IsBool()) {
		throw IotaRequestException("isHealthy in response missing or not a boolean", fullPath);
	}
	result.isHealthy = data["isHealthy"].GetBool();
	if (!data.HasMember("messagesPerSecond") || !data["messagesPerSecond"].IsBool()) {
		throw IotaRequestException("messagesPerSecond in response missing or not a boolean", fullPath);
	}
	result.messagesPerSecond = data["messagesPerSecond"].GetFloat();
	if (!data.HasMember("confirmedMilestoneIndex") || !data["confirmedMilestoneIndex"].IsUint()) {
		throw IotaRequestException("confirmedMilestoneIndex in response missing or not a uint", fullPath);
	}
	result.confirmedMilestoneIndex = data["confirmedMilestoneIndex"].GetUint();
	if (!data.HasMember("latestMilestoneIndex") || !data["latestMilestoneIndex"].IsInt()) {
		throw IotaRequestException("latestMilestoneIndex in response missing or not a uint", fullPath);
	}
	result.lastMilestoneIndex = data["latestMilestoneIndex"].GetInt();
	if (!data.HasMember("latestMilestoneTimestamp") || !data["latestMilestoneTimestamp"].IsInt64()) {
		throw IotaRequestException("latestMilestoneTimestamp in response missing or not a int64", fullPath);
	}
	result.lastMilestoneTimestamp = data["latestMilestoneTimestamp"].GetInt64();
	
	return result;
}


void IotaRequest::defaultExceptionHandler(Poco::Logger& errorLog, bool terminate/* = true*/)
{
	std::pair<std::string, std::unique_ptr<std::string>> dataIndex;
	try {
		throw; // assumes that defaultExceptionHandler() is called from catch-clause
	}
	catch (IotaRequestException& ex) {
		// terminate application
		errorLog.critical("Iota Request exception, has the API changed? More details: %s", ex.getFullString());
		if (terminate) {
			Poco::Util::ServerApplication::terminate();
		}
	}
	catch (RapidjsonParseErrorException& ex) {
		errorLog.error("Json parse error by calling Iota API: %s", ex.getFullString());
		if (terminate) {
			Poco::Util::ServerApplication::terminate();
		}
		
	}
	catch (PocoNetException& ex) {
		errorLog.error("Poco Net Exception by calling Iota Request: %s", ex.getFullString());
		if (terminate) {
			Poco::Util::ServerApplication::terminate();
		}
	}
	catch (Poco::Exception& ex) {
		errorLog.error("Poco Exception by calling Iota Request: %s", ex.displayText());
		if (terminate) {
			Poco::Util::ServerApplication::terminate();
		}
	}
}