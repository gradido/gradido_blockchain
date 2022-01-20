#include "JsonCakePHPRequest.h"

#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/DateTimeFormatter.h"

#include "JsonRequestHandler.h"

#include "rapidjson/writer.h"
using namespace rapidjson;

JsonCakePHPRequest::JsonCakePHPRequest(const std::string& serverHost, int serverPort)
	: JsonRequest(serverHost, serverPort)
{

}

JsonRequestReturn JsonCakePHPRequest::request(const char* methodName)
{
	static const char* functionName = "JsonRequest::request";

	auto alloc = mJsonDocument.GetAllocator();
	mJsonDocument.AddMember("method", Value(methodName, alloc), alloc);

	auto responseString = POST("/JsonRequestHandler", mJsonDocument);
	if (responseString == "client session zero") {
		addError(new Error(functionName, "server host or server port not given"));
		return JSON_REQUEST_PARAMETER_ERROR;
	}

	try {
		// extract parameter from request
		Document resultJson;
		resultJson.Parse(responseString.data());

		if (resultJson.HasParseError())
		{
			addError(new ParamError(functionName, "error parsing request answer", resultJson.GetParseError()));
			addError(new ParamError(functionName, "position of last parsing error", resultJson.GetErrorOffset()));
			std::string dateTimeString = Poco::DateTimeFormatter::format(Poco::DateTime(), "%d%m%yT%H%M%S");
			std::string log_Path = "/var/log/grd_login/";
			//#ifdef _WIN32
#if defined(_WIN32) || defined(_WIN64)
			log_Path = "./";
#endif
			std::string fileName = log_Path + dateTimeString + "_response_";
			fileName += methodName;
			fileName += ".html";

			FILE* f = fopen(fileName.data(), "wt");
			if (f) {
				fwrite(responseString.data(), 1, responseString.size(), f);
				fclose(f);
			}
			//	*/
			sendErrorsAsEmail(responseString);
			return JSON_REQUEST_RETURN_PARSE_ERROR;
		}

		if (!resultJson.HasMember("state") && resultJson.HasMember("message")) {
			// than we have maybe get an cakephp exception as result
			std::string message;
			JsonRequestHandler::getStringParameter(resultJson, "message", message);
			addError(new ParamError(functionName, "cakePHP Exception: ", message));
			addError(new ParamError(functionName, "calling: ", methodName));
			addError(new ParamError(functionName, "for server host: ", mRequestUri.getHost()));
			std::string fields[] = { "url", "code", "file", "line" };
			for (int i = 0; i < 4; i++) {
				auto field = fields[i];
				std::string field_name = field + ": ";
				std::string value;
				Value::ConstMemberIterator itr = resultJson.FindMember(field.data());
				if (itr != resultJson.MemberEnd()) {
					if (itr->value.IsString()) {
						value = itr->value.GetString();
					}
					else if (itr->value.IsInt()) {
						value = std::to_string(itr->value.GetInt());
					}
					addError(new ParamError(functionName, field_name.data(), value));
				}
				JsonRequestHandler::getStringParameter(resultJson, field.data(), value);

			}
			sendErrorsAsEmail("", true);
			return JSON_REQUEST_RETURN_ERROR;
		}
		else {
			std::string stateString;
			JsonRequestHandler::getStringParameter(resultJson, "state", stateString);
			if (stateString == "error") {
				addError(new Error(functionName, "php server return error"));
				std::string msg;
				JsonRequestHandler::getStringParameter(resultJson, "msg", msg);
				if (msg != "") {
					addError(new ParamError(functionName, "msg:", msg));
				}

				std::string details;
				JsonRequestHandler::getStringParameter(resultJson, "details", details);
				if (details != "") {
					addError(new ParamError(functionName, "details:", details));
				}
				else {
					Value::ConstMemberIterator itr = resultJson.FindMember("details");
					if (itr != resultJson.MemberEnd()) {
						StringBuffer buffer;
						Writer<StringBuffer> writer(buffer);
						const auto& obj = itr->value;
						obj.Accept(writer);
						addError(new ParamError(functionName, "details", buffer.GetString()));
					}
				}
				sendErrorsAsEmail("", true);
				return JSON_REQUEST_RETURN_ERROR;
			}
			else if (stateString == "success") {
				auto it = resultJson.FindMember("warnings");
				if (it != resultJson.MemberEnd()) {
					const Value& warnings = it->value;
					for (auto it = warnings.MemberBegin(); it != warnings.MemberEnd(); it++) {
						if (!it->name.IsString() || !it->value.IsString()) {
							continue;
						}
						std::string name(it->name.GetString(), it->name.GetStringLength());
						std::string value(it->value.GetString(), it->value.GetStringLength());

						addWarning(new Warning(name, value));
					}
				}
			}
		}
	}
	catch (Poco::Exception& e) {
		addError(new ParamError(functionName, "connect error to php server", e.displayText().data()));
		addError(new ParamError(functionName, "host", mRequestUri.getHost()));
		addError(new ParamError(functionName, "port", mRequestUri.getPort()));
		sendErrorsAsEmail();
		return JSON_REQUEST_CONNECT_ERROR;
	}

	return JSON_REQUEST_RETURN_OK;
}
JsonRequestReturn JsonCakePHPRequest::request(const char* methodName, rapidjson::Value& payload)
{
	auto alloc = mJsonDocument.GetAllocator();
	if (payload.IsObject()) {

		for (auto it = payload.MemberBegin(); it != payload.MemberEnd(); it++) {
			mJsonDocument.AddMember(it->name, it->value, alloc);
		}
	}
	return request(methodName);
}