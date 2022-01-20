#include "JsonRPCRequest.h"

#include "JsonRequestHandler.h"

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/pointer.h"

using namespace rapidjson;

JsonRPCRequest::JsonRPCRequest(const std::string& serverHost, int serverPort)
	: JsonRequest(Poco::URI(serverHost).getHost(), serverPort)
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
	try {
		auto responseString = POST("/", requestJson, "HTTP/1.1");
		// debugging answer
		if (responseString.size() == 0) {
			addError(new Error(functionName, "empty request answer"));
			addError(new ParamError(functionName, "server host: ", mRequestUri.getHost()));
			addError(new ParamError(functionName, "server port", mRequestUri.getPort()));
			addError(new ParamError(functionName, "method: ", methodName));
			sendErrorsAsEmail();
			return nullptr;
		}

		auto jsonAnswear = parseResponse(responseString);

		if (!jsonAnswear.IsObject())
		{
			std::string fileName = "node_response_";
			fileName += methodName;
			fileName += ".html";

			FILE* f = fopen(fileName.data(), "wt");
			if (f) {
				fwrite(responseString.data(), 1, responseString.size(), f);
				fclose(f);
			}
			//	*/
			sendErrorsAsEmail(responseString);
			return nullptr;
		}

		std::string state;
		Value& stateValue = Pointer("/result/state").GetWithDefault(jsonAnswear, "");
		if (stateValue.GetStringLength()) {
			state = stateValue.GetString();
		}

		if (state == "error") {
			addError(new Error(functionName, "node server return error"));
			Value& msg = Pointer("/result/msg").GetWithDefault(jsonAnswear, "");
			if (msg.GetStringLength()) {
				addError(new ParamError(functionName, "msg:", msg.GetString()));
			}
			Value& details = Pointer("/result/details").GetWithDefault(jsonAnswear, "");
			if (details.GetStringLength()) {
				addError(new ParamError(functionName, "details:", details.GetString()));
			}
			sendErrorsAsEmail();
			return nullptr;
		}
		else if (state == "success") {
			/*for (auto it = result->begin(); it != result->end(); it++) {
				std::string index = it->first;
				std::string value = it->second.toString();
				printf("[JsonRequest] %s: %s\n", index.data(), value.data());
			}*/
			return jsonAnswear;
		}
	}
	catch (Poco::Exception& e) {
		addError(new ParamError(functionName, "connect error to node server", e.displayText().data()));
		sendErrorsAsEmail();
		return nullptr;
	}

	return nullptr;
}