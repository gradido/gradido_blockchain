#include "gradido_blockchain/http/IotaRequest.h"
#include "gradido_blockchain/http/IotaRequestExceptions.h"
#include "gradido_blockchain/http/RequestExceptions.h"
#include "gradido_blockchain/http/ServerConfig.h"
#include "gradido_blockchain/ServerApplication.h"

#include "gradido_blockchain/lib/Profiler.h"

#include <algorithm>
#include <cctype>

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/pointer.h"

#include "furi/furi.hpp"
#ifdef USE_HTTPS
#define CPPHTTPLIB_OPENSSL_SUPPORT
#endif
#include "httplib.h"
#include "loguru/loguru.hpp"

#ifdef ENABLE_IOTA_RUST_CLIENT
#include "iota_rust_clib.h"
#endif

using namespace rapidjson;


IotaRequest::IotaRequest(const std::string& serverHost, int serverPort, const std::string& urlPath)
	: JsonRequest(serverHost, serverPort)
{
	mUrl += urlPath;
}

IotaRequest::~IotaRequest()
{

}

std::vector<std::string> IotaRequest::getTips()
{
	auto fullPath = buildFullPath("tips");
	auto responseString = GET(fullPath);

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



std::string IotaRequest::sendMessage(const iota::TopicIndex& index, const std::string& messageHex)
{
	if (ServerConfig::g_IotaLocalPow) {
		//auto index = DataTypeConverter::hexToBinString(indexHex.substr(0, indexHex.size()-1));
		//auto message = DataTypeConverter::hexToBinString(messageHex.substr(0, messageHex.size()-1));
		//printf("message hex: %s\n", messageHex.data());
#ifdef ENABLE_IOTA_RUST_CLIENT
		return sendMessageViaRustIotaClient(index.getBinString(), messageHex);
#else 
		throw IotaRustClientMissingException("Gradido Blockchain was builded without iota rust client, please set iota.local_pow to false or rebuild with iota rust client");
#endif 
	}
	auto tips = getTips();
	if (!tips.size()) {
		throw IotaRequestException("no tips", buildFullPath("tips"));
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

	payload.AddMember("index", Value(index.getHexString().data(), alloc), alloc);
	payload.AddMember("data", Value(messageHex.data(), alloc), alloc);

	requestJson.AddMember("payload", payload, alloc);
	// TODO: calculate nonce
	requestJson.AddMember("nonce", "", alloc);

	auto fullPath = buildFullPath("messages");
	auto responseString = POST(fullPath.data(), requestJson);

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

	std::string fullPath = buildFullPath(std::string("messages", 8), messageIdHex);
	return parseResponse(GET(fullPath.data()));
}

std::pair<std::unique_ptr<std::string>, std::unique_ptr<std::string>> IotaRequest::getIndexiationMessageDataIndex(const std::string& messageIdHex)
{
	auto json = getMessageJson(messageIdHex);
	if (!json.IsObject() || json.FindMember("data") == json.MemberEnd()) {
		throw IotaRequestException("error calling messages from iota, no valid json or no data member found", buildFullPath(std::string("messages", 8), messageIdHex));
	}
	auto payload = Pointer("/data/payload").Get(json);
	if (!payload->HasMember("data") || !payload->HasMember("index")) {
		throw IotaRequestException("json field data.payload.data or data.payload.index missing", buildFullPath(std::string("messages", 8), messageIdHex));
	}
	if (!(*payload)["data"].GetStringLength()) {
		return { std::make_unique<std::string>(), DataTypeConverter::hexToBinString((*payload)["index"].GetString())};
	}
	return { DataTypeConverter::hexToBinString((*payload)["data"].GetString()), DataTypeConverter::hexToBinString((*payload)["index"].GetString()) };
}

uint32_t IotaRequest::getMessageMilestoneId(const std::string& messageIdHex)
{
	// GET /api/v1/messages/{messageId}/metadata
	std::string fullPath = buildFullPath(std::string("messages", 8), messageIdHex, std::string("metadata", 8));

	auto json = parseResponse(GET(fullPath.data()));

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

std::vector<memory::Block> IotaRequest::findByIndex(const iota::TopicIndex& index)
{
	// GET /api/v1/messages?index=4752414449444f2e7079746861676f726173
	auto pathView = extractPathFromUrl();
	std::string fullPath;
	auto indexHex = index.getHexString();
	fullPath.reserve(pathView.size() + 18 + indexHex.size());
	fullPath = std::string(pathView) + "messages?index=" + indexHex;
	std::vector<memory::Block> result;

	auto json = parseResponse(GET(fullPath.data()));
	if (!json.IsObject() || !json.HasMember("data")) {
		throw IotaRequestException("findByIndex failed or data not set", fullPath);
	}

	Value& data = json["data"];
	if (!data.HasMember("messageIds") || !data["messageIds"].IsArray()) {
		throw IotaPostRequestException("messageIds not found or not an array", fullPath);
	}
	auto messageIds = data["messageIds"].GetArray();
	for (auto it = messageIds.Begin(); it != messageIds.End(); ++it) {
		result.push_back(memory::Block::fromHex(it->GetString(), data["messageId"].GetStringLength()));
	}

	return result;
}

memory::Block IotaRequest::getMilestoneByIndex(int32_t milestoneIndex)
{
	// GET api/v1/milestones/909039
	std::string fullPath = buildFullPath(std::string("milestones", 10), std::to_string(milestoneIndex));

	auto json = parseResponse(GET(fullPath.data()));
	if (!json.IsObject() || !json.HasMember("data")) {
		throw IotaRequestException("getMilestoneByIndex failed or data not set", fullPath);
	}
	Value& data = json["data"];
	if (!data.HasMember("messageId") || !data["messageId"].IsString()) {
		throw IotaRequestException("messageId field not set or not string", fullPath);
	}
	return memory::Block::fromHex(data["messageId"].GetString(), data["messageId"].GetStringLength());
}

uint64_t IotaRequest::getMilestoneTimestamp(int32_t milestoneIndex)
{
	// GET api/v1/milestones/909039
	std::string fullPath = buildFullPath(std::string("milestones", 10), std::to_string(milestoneIndex));

	auto json = parseResponse(GET(fullPath.data()));
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
	auto fullPath = buildFullPath(std::string("info", 4));
	auto json = parseResponse(GET(fullPath.data()));
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


void IotaRequest::defaultExceptionHandler(bool terminate/* = true*/)
{
	std::pair<std::string, std::unique_ptr<std::string>> dataIndex;
	try {
		throw; // assumes that defaultExceptionHandler() is called from catch-clause
	}
	catch (IotaRequestException& ex) {
		// terminate application
		ABORT_F("Iota Request exception, has the API changed? More details: %s", ex.getFullString().data());
		if (terminate) {
			ServerApplication::terminate();
		}
	}
	catch (RapidjsonParseErrorException& ex) {
		LOG_F(ERROR, "Json parse error by calling Iota API: %s", ex.getFullString().data());
		if (terminate) {
			ServerApplication::terminate();
		}
	}
	catch (HttplibRequestException& ex) {
		LOG_F(ERROR, "Http lib error by calling Iota API: %s", ex.getFullString().data());
		if (terminate) {
			ServerApplication::terminate();
		}
	}
}
#ifdef ENABLE_IOTA_RUST_CLIENT
std::string IotaRequest::sendMessageViaRustIotaClient(const std::string& index, const std::string& message)
{
	std::string iotaUrl;
	auto uri = furi::uri_split::from_uri(mUrl);
	auto authority = furi::authority_split::from_authority(uri.authority);
	if (authority.port == "443") {
		iotaUrl = "https://";
	}
	else {
		iotaUrl = "http://";
	}
	iotaUrl += uri.authority;
	auto result = iota_send_indiced_transaction(iotaUrl.data(), index.data(), message.data());
	std::string resultJsonString = result;
	Document resultJson;
	resultJson.Parse(result);
	free_rust_string(result);

	// analyse result
	auto stateIt = resultJson.FindMember("state");
	if (stateIt == resultJson.MemberEnd()) {
		throw IotaRequestException("state in response missing", iotaUrl);
	}
	std::string stateString = stateIt->value.GetString();
	if (stateString != "success") {
		throw IotaRequestException(resultJson["msg"].GetString(), iotaUrl);
	}
	return std::move(std::string(resultJson["message_id"].GetString()));
}
#endif

std::string_view IotaRequest::extractPathFromUrl()
{
	auto uri = furi::uri_split::from_uri(mUrl);
	return uri.path;
}

std::string IotaRequest::buildFullPath(const std::string& first, const std::string& second/* = ""*/, const std::string& third /*= ""*/)
{
	auto pathView = extractPathFromUrl();
	int stringSize = pathView.size() + first.size();
	if (second.size()) {
		stringSize += second.size() + 1;
	}
	if (third.size()) {
		stringSize += third.size() + 1;
	}
	std::string result;
	result.reserve(stringSize);
	result = std::string(pathView);
	result += first;
	if (second.size()) {
		result += '/' + second;
	}
	if (third.size()) {
		result += '/' + third;
	}
	return result;
}
