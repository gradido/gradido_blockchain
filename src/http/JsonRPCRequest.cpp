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
	static const char* functionName = "JsonRPCRequest::request";
		
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

	std::string state;
	Value& stateValue = Pointer("/result/state").GetWithDefault(jsonAnswear, "");
	if (stateValue.GetStringLength()) {
		state = stateValue.GetString();
	}

	std::string errorMsg, detailsString;
	Value& msg = Pointer("/result/msg").GetWithDefault(jsonAnswear, "");
	if (msg.GetStringLength()) {
		errorMsg = msg.GetString();
	}
	
	if (state == "error") {
		RequestResponseErrorException exception("node server return error", mRequestUri, errorMsg);
		throw exception.setDetails(Pointer("/result/details").GetWithDefault(jsonAnswear, ""));
	}
	else if (state == "success") {
		/*for (auto it = result->begin(); it != result->end(); it++) {
			std::string index = it->first;
			std::string value = it->second.toString();
			printf("[JsonRequest] %s: %s\n", index.data(), value.data());
		}*/
		return jsonAnswear;
	}
	RequestResponseErrorException exception("node server return unhandled state", mRequestUri, errorMsg);
	throw exception.setDetails(detailsString);
}