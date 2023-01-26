#include "gradido_blockchain/http/JsonRPCRequest.h"

#include "gradido_blockchain/http/JsonRequestHandler.h"
#include "gradido_blockchain/http/RequestExceptions.h"

#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/pointer.h"

using namespace rapidjson;

JsonRPCRequest::JsonRPCRequest(const std::string& serverHost, int serverPort)
	: JsonRequest(Poco::URI(serverHost).getHost(), serverPort)
{

}

JsonRPCRequest::JsonRPCRequest(const Poco::URI& serverHost)
	: JsonRequest(serverHost)
{

}

JsonRPCRequest::~JsonRPCRequest()
{

}

Document JsonRPCRequest::request(const char* methodName, Value& params)
{
	Document requestJson(kObjectType);
	auto alloc = requestJson.GetAllocator();

	requestJson.AddMember("jsonrpc", "2.0", alloc);
	requestJson.AddMember("id", rand(), alloc);
	requestJson.AddMember("method", Value(methodName, alloc), alloc);
	requestJson.AddMember("params", params, alloc);

	// TODO: use group alias instead of /
	
	auto responseString = POST("/", requestJson, "HTTP/1.1");
	// debugging answer
	if (responseString.size() == 0) {
		throw RequestEmptyResponseException("methodName", mRequestUri);
	}

	auto jsonAnswear = parseResponse(responseString);

	if (!jsonAnswear.IsObject()) {
		throw RequestResponseInvalidJsonException(methodName, mRequestUri, responseString);
	}
		
	if (jsonAnswear.FindMember("error") != jsonAnswear.MemberEnd()) {
		throw JsonRPCException(
			jsonAnswear["error"]["message"].GetString(),
			static_cast<JsonRPCErrorCodes>(jsonAnswear["error"]["code"].GetInt())
		);
	}	
	
	return std::move(jsonAnswear);
}

std::vector<Value> JsonRPCRequest::batchRequest(std::vector<std::string> methods, std::vector<Value> params)
{
	assert(methods.size() == params.size());

	std::vector<Value> results;
	std::map<int, Value> orderResultsMap;
	results.reserve(methods.size());
	int startId = rand();
	Document batchRequestJson(kArrayType);
	auto alloc = batchRequestJson.GetAllocator();

	for (int i = 0; i < methods.size(); i++) {
		Value requestJson(kObjectType);
		requestJson.AddMember("jsonrpc", "2.0", alloc);
		requestJson.AddMember("id", startId+i, alloc);
		requestJson.AddMember("method", Value(methods[i].data(), alloc), alloc);
		requestJson.AddMember("params", params[i], alloc);
		batchRequestJson.PushBack(requestJson, alloc);
	}

	auto responseString = POST("/", batchRequestJson, "HTTP/1.1");
	// debugging answer
	if (responseString.size() == 0) {
		throw RequestEmptyResponseException("methodName", mRequestUri);
	}

	auto jsonAnswearBatch = parseResponse(responseString);

	if (!jsonAnswearBatch.IsArray()) {
		throw RequestResponseInvalidJsonException("batch", mRequestUri, responseString);
	}

	for (auto& jsonAnswear : jsonAnswearBatch.GetArray())
	{
		if (jsonAnswear.FindMember("error") != jsonAnswear.MemberEnd()) {
			throw JsonRPCException(
				jsonAnswear["error"]["message"].GetString(),
				static_cast<JsonRPCErrorCodes>(jsonAnswear["error"]["code"].GetInt())
			);
		}
		if (!jsonAnswear.HasMember("id") || !jsonAnswear["id"].IsInt()) {
			throw JsonRPCException(
				"id in response not set correctly",
				JSON_RPC_ERROR_INVALID_PARAMS
			);
		}
		orderResultsMap.insert({ jsonAnswear["id"].GetInt(), std::move(jsonAnswear) });
	}
	for (auto& pair : orderResultsMap) {
		results.push_back(std::move(pair.second));
	}
	return results;
}