#include "gradido_blockchain/http/JsonRequestHandler.h"

#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"

#include "Poco/DeflatingStream.h"

#include "gradido_blockchain/http/ServerConfig.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

#include "gradido_blockchain/GradidoBlockchainException.h"

#include <sstream>

using namespace rapidjson;


JsonRequestHandler::JsonRequestHandler()
{

}


JsonRequestHandler::JsonRequestHandler(Poco::Net::IPAddress clientIp)
	:  mClientIp(clientIp)
{

}


void JsonRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
	mClientIp = request.clientAddress().host();
	
	if (request.secure()) {
		mServerHost = "https://" + request.getHost();
	}
	else {
		mServerHost = "http://" + request.getHost();
	}
	auto method = request.getMethod();
	std::istream& request_stream = request.stream();
	Document rapid_json_result;
	Document rapidjson_params;
	if (method == "POST" || method == "PUT") {
		// extract parameter from request
		parseJsonWithErrorPrintFile(request_stream, rapidjson_params);
	    
		if (rapidjson_params.IsObject()) {
			try {
				rapid_json_result = handle(rapidjson_params);
			}
			catch (GradidoBlockchainException& ex) {
				rapid_json_result = stateError(ex.getFullString().data());
			}
		}
		else {
			rapid_json_result = stateError("empty body");
		}
	}
	else if(method == "GET") {		
		Poco::URI uri(request.getURI());
		parseQueryParametersToRapidjson(uri, rapidjson_params);
		
		try {
			rapid_json_result = handle(rapidjson_params);
		}
		catch (GradidoBlockchainException& ex)
		{
			rapid_json_result = stateError(ex.getFullString().data());
		}
	}

	if (!rapid_json_result.IsNull()) {
		responseWithJson(rapid_json_result, request, response);
	}	
}

void JsonRequestHandler::responseWithJson(const rapidjson::Document& json, Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
	bool _compressResponse(request.hasToken("Accept-Encoding", "gzip"));

	response.setChunkedTransferEncoding(false);
	response.setContentType("application/json");	
	if (ServerConfig::g_AllowUnsecureFlags & ServerConfig::UNSECURE_CORS_ALL) {
		response.set("Access-Control-Allow-Origin", "*");
		response.set("Access-Control-Allow-Headers", "Authorization, Access-Control-Allow-Headers, Origin,Accept, X-Requested-With, Content-Type, Access-Control-Request-Method, Access-Control-Request-Headers");
	}

	if (_compressResponse) { response.set("Content-Encoding", "gzip"); }
	std::ostream& _responseStream = response.send();
	Poco::DeflatingOutputStream _gzipStream(_responseStream, Poco::DeflatingStreamBuf::STREAM_GZIP, 1);
	std::ostream& responseStream = _compressResponse ? _gzipStream : _responseStream;
	
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	json.Accept(writer);

	responseStream << buffer.GetString() << std::endl;

	if (_compressResponse) _gzipStream.close();
}

void JsonRequestHandler::responseOptions(Poco::Net::HTTPServerResponse& response)
{
	if (ServerConfig::g_AllowUnsecureFlags & ServerConfig::UNSECURE_CORS_ALL) {
		response.set("Access-Control-Allow-Origin", "*");
		response.set("Access-Control-Allow-Headers", "Authorization, Access-Control-Allow-Headers, Origin,Accept, X-Requested-With, Content-Type, Access-Control-Request-Method, Access-Control-Request-Headers");
	}
	response.send();
}

bool JsonRequestHandler::parseQueryParametersToRapidjson(const Poco::URI& uri, Document& rapidParams)
{
	auto queryParameters = uri.getQueryParameters();
	rapidParams.SetObject();
	for (auto it = queryParameters.begin(); it != queryParameters.end(); it++) {
		int tempi = 0;
		Value name_field(it->first.data(), rapidParams.GetAllocator());
		if (DataTypeConverter::NUMBER_PARSE_OKAY == DataTypeConverter::strToInt(it->second, tempi)) {
			//rapidParams[it->first.data()] = rapidjson::Value(tempi, rapidParams.GetAllocator());
			rapidParams.AddMember(name_field.Move(), tempi, rapidParams.GetAllocator());
		}
		else {
			rapidParams.AddMember(name_field.Move(), Value(it->second.data(), rapidParams.GetAllocator()), rapidParams.GetAllocator());
		}
	}
	
	return true;
}

void JsonRequestHandler::parseJsonWithErrorPrintFile(std::istream& request_stream, Document& rapidParams, const char* functionName /* = nullptr*/)
{
	// debugging answer

	std::stringstream responseStringStream;
	for (std::string line; std::getline(request_stream, line); ) {
		responseStringStream << line << std::endl;
	}

	rapidParams.Parse(responseStringStream.str().data());
	if (rapidParams.HasParseError()) {
		throw RapidjsonParseErrorException("error parsing request answer", rapidParams.GetParseError(), rapidParams.GetErrorOffset())
			.setRawText(responseStringStream.str());
	}

}

Document JsonRequestHandler::stateError(const char* msg, std::string details)
{
	Document obj(kObjectType);
	auto alloc = obj.GetAllocator();
	obj.AddMember("state", "error", alloc);
	obj.AddMember("msg", Value(msg, alloc), alloc);
	
	if (details.size()) {
		obj.AddMember("details", Value(details.data(), alloc), alloc);
	}

	return obj;
}

Document JsonRequestHandler::stateError(const char* msg, GradidoBlockchainException& ex)
{
	Document obj(kObjectType);
	auto alloc = obj.GetAllocator();
	obj.AddMember("state", "error", alloc);
	obj.AddMember("msg", Value(msg, alloc), alloc);
	obj.AddMember("details", ex.getDetails(alloc), alloc);
	return obj;
}


Document JsonRequestHandler::stateSuccess()
{
	Document obj(kObjectType);
	obj.AddMember("state", "success", obj.GetAllocator());

	return obj;
}


Document JsonRequestHandler::customStateError(const char* state, const char* msg, std::string details /* = "" */ )
{
	Document obj(kObjectType);
	obj.AddMember("state", Value(state, obj.GetAllocator()).Move(), obj.GetAllocator());
	obj.AddMember("msg", Value(msg, obj.GetAllocator()).Move(), obj.GetAllocator());
	
	if (details.size()) {
		obj.AddMember("details", Value(details.data(), obj.GetAllocator()).Move(), obj.GetAllocator());
	}
	return obj;
}


Document JsonRequestHandler::stateWarning(const char* msg, std::string details/* = ""*/)
{
	Document obj(kObjectType);
	obj.AddMember("state", "warning", obj.GetAllocator());
	obj.AddMember("msg", Value(msg, obj.GetAllocator()).Move(), obj.GetAllocator());

	if (details.size()) {
		obj.AddMember("details", Value(details.data(), obj.GetAllocator()).Move(), obj.GetAllocator());
	}

	return obj;
}



Document JsonRequestHandler::getIntParameter(const Document& params, const char* fieldName, int& iparameter)
{
	Value::ConstMemberIterator itr = params.FindMember(fieldName);
	std::string message = fieldName;
	if (itr == params.MemberEnd()) {
		message += " not found";
		return stateError(message.data());
	}
	if (!itr->value.IsInt()) {
		message = "invalid " + message;
		return stateError(message.data());
	}
	iparameter = itr->value.GetInt();
	return Document();
}

Document JsonRequestHandler::getBoolParameter(const rapidjson::Document& params, const char* fieldName, bool& bParameter)
{
	Value::ConstMemberIterator itr = params.FindMember(fieldName);
	std::string message = fieldName;
	if (itr == params.MemberEnd()) {
		message += " not found";
		return stateError(message.data());
	}
	if (!itr->value.IsBool()) {
		message = "invalid " + message;
		return stateError(message.data());
	}
	bParameter = itr->value.GetBool();
	return Document();
}

Document JsonRequestHandler::getInt64Parameter(const rapidjson::Document& params, const char* fieldName, Poco::Int64& iParameter)
{
	Value::ConstMemberIterator itr = params.FindMember(fieldName);
	std::string message = fieldName;
	if (itr == params.MemberEnd()) {
		message += " not found";
		return stateError(message.data());
	}
	if (!itr->value.IsInt64()) {
		message = "invalid " + message;
		return stateError(message.data());
	}
	iParameter = itr->value.GetInt64();
	return Document();
}

Document JsonRequestHandler::getUIntParameter(const Document& params, const char* fieldName, unsigned int& iParameter)
{
	Value::ConstMemberIterator itr = params.FindMember(fieldName);
	std::string message = fieldName;
	if (itr == params.MemberEnd()) {
		message += " not found";
		return stateError(message.data());
	}
	if (!itr->value.IsUint()) {
		message = "invalid " + message;
		return stateError(message.data());
	}
	iParameter = itr->value.GetUint();
	return Document();
}

Document JsonRequestHandler::getUInt64Parameter(const Document& params, const char* fieldName, Poco::UInt64& iParameter)
{
	Value::ConstMemberIterator itr = params.FindMember(fieldName);
	std::string message = fieldName;
	if (itr == params.MemberEnd()) {
		message += " not found";
		return stateError(message.data());
	}
	if (!itr->value.IsUint64()) {
		message = "invalid " + message;
		return stateError(message.data());
	}
	iParameter = itr->value.GetUint64();
	return Document();
}
Document JsonRequestHandler::getStringParameter(const Value& params, const char* fieldName, std::string& strParameter)
{
	Value::ConstMemberIterator itr = params.FindMember(fieldName);
	std::string message = fieldName;
	if (itr == params.MemberEnd()) {
		message += " not found";
		return stateError(message.data());
	}
	if (!itr->value.IsString()) {
		message = "invalid " + message;
		return stateError(message.data());
	}
	strParameter = std::string(itr->value.GetString(), itr->value.GetStringLength());
	return Document();
}

Document JsonRequestHandler::getStringIntParameter(const Document& params, const char* fieldName, std::string& strParameter, int& iParameter)
{
	Value::ConstMemberIterator itr = params.FindMember(fieldName);
	std::string message = fieldName;
	if (itr == params.MemberEnd()) {
		message += " not found";
		return stateError(message.data());
	}
	if (itr->value.IsString()) {
		strParameter = std::string(itr->value.GetString(), itr->value.GetStringLength());
	}
	else if (itr->value.IsInt()) {
		iParameter = itr->value.GetInt();
	}
	else {
		message += " isn't neither int or string"; 
		return stateError(message.data());
	}
	
	return Document();
}

Document JsonRequestHandler::checkArrayParameter(const Document& params, const char* fieldName)
{
	
	Value::ConstMemberIterator itr = params.FindMember(fieldName);
	std::string message = fieldName;
	if (itr == params.MemberEnd()) {
		message += " not found";
		return stateError(message.data());
	}
	
	if (!itr->value.IsArray()) {
		message += " is not a array";
		return stateError(message.data());
	}

	return Document();
}

Document JsonRequestHandler::checkObjectParameter(const Document& params, const char* fieldName)
{
	Value::ConstMemberIterator itr = params.FindMember(fieldName);
	std::string message = fieldName;
	if (itr == params.MemberEnd()) {
		message += " not found";
		return stateError(message.data());
	}

	if (!itr->value.IsObject()) {
		message += " is not a object";
		return stateError(message.data());
	}

	return Document();
}

