#include "gradido_blockchain/http/JsonCakePHPRequest.h"
#include "gradido_blockchain/http/RequestExceptions.h"
#include "gradido_blockchain/lib/RapidjsonHelper.h"

#include "rapidjson/writer.h"
using namespace rapidjson;

JsonCakePHPRequest::JsonCakePHPRequest(const std::string& serverHost, int serverPort)
	: JsonRequest(serverHost, serverPort)
{

}

bool JsonCakePHPRequest::request(const char* methodName)
{
	static const char* functionName = "JsonRequest::request";

	auto alloc = mJsonDocument.GetAllocator();
	mJsonDocument.AddMember("method", Value(methodName, alloc), alloc);

	auto responseString = POST("/JsonRequestHandler", mJsonDocument);
	
	// extract parameter from request
	Document resultJson = parseResponse(responseString);

	if (!resultJson.HasMember("state") && resultJson.HasMember("message")) {
		// than we have maybe get an cakephp exception as result
		std::string message;
		if (resultJson["message"].IsString()) {
			message = resultJson["message"].GetString();
		}
		throw RequestResponseCakePHPException(methodName, mUrl, message)
			.setDetails(resultJson);
	}
	else {
		rapidjson_helper::checkMember(resultJson, "state", rapidjson_helper::MemberType::STRING, "cakePHP Response");
		std::string stateString = resultJson["state"].GetString();

		if (stateString == "error") {
			std::string msg;
			if (resultJson.HasMember("msg") && resultJson["msg"].IsString()) {
				msg = resultJson["msg"].GetString();
			}
			
			RequestResponseErrorException exception("php server return error", mUrl, msg);
			std::string details;
			if (resultJson.HasMember("details")) {
				if (resultJson["details"].IsString()) {
					exception.setDetails(details);
				}
				else {
					Value::ConstMemberIterator itr = resultJson.FindMember("details");
					if (itr != resultJson.MemberEnd()) {
						StringBuffer buffer;
						Writer<StringBuffer> writer(buffer);
						const auto& obj = itr->value;
						obj.Accept(writer);
						details = buffer.GetString();
					}
				}
			}
			throw exception;
		}
		else if (stateString == "success") {
			auto it = resultJson.FindMember("warnings");
			if (it != resultJson.MemberEnd()) {
				printf("JsonCakePHP request response for %s has warnings:\n", methodName);
				const Value& warnings = it->value;
				for (auto it = warnings.MemberBegin(); it != warnings.MemberEnd(); it++) {
					if (!it->name.IsString() || !it->value.IsString()) {
						continue;
					}
					std::string name(it->name.GetString(), it->name.GetStringLength());
					std::string value(it->value.GetString(), it->value.GetStringLength());	
					printf("warning: %s: %s\n", name.data(), value.data());
				}
			}
		}
	}	

	return true;
}
bool JsonCakePHPRequest::request(const char* methodName, rapidjson::Value& payload)
{
	auto alloc = mJsonDocument.GetAllocator();
	if (payload.IsObject()) {

		for (auto it = payload.MemberBegin(); it != payload.MemberEnd(); it++) {
			mJsonDocument.AddMember(it->name, it->value, alloc);
		}
	}
	return request(methodName);
}