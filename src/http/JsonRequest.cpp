
#include "gradido_blockchain/http/JsonRequest.h"

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "gradido_blockchain/http/RequestExceptions.h"

#include "furi/furi.hpp"
#include "magic_enum/magic_enum.hpp"
#ifndef _DEBUG
#define CPPHTTPLIB_OPENSSL_SUPPORT
#endif
#include "httplib.h"

using namespace rapidjson;

JsonRequest::JsonRequest(const std::string& serverHost, int serverPort)
	: HttpRequest(serverHost, serverPort), mJsonDocument(kObjectType)
{

}

JsonRequest::JsonRequest(const std::string& requestUri)
	: HttpRequest(requestUri), mJsonDocument(kObjectType)
{

}

JsonRequest::~JsonRequest()
{

}

Document JsonRequest::postRequest(const char* path, Value& payload)
{
	auto alloc = mJsonDocument.GetAllocator();

	if (payload.IsObject()) {
		for (auto it = payload.MemberBegin(); it != payload.MemberEnd(); it++) {
			mJsonDocument.AddMember(it->name, it->value, alloc);
		}
	}
	auto responseString = POST(path, mJsonDocument);
	auto responseJson = parseResponse(responseString);
	
	return responseJson;
}

Document JsonRequest::getRequest(const char* path)
{
	auto responseString = GET(path);
	auto responseJson = parseResponse(responseString);
	return responseJson;

}

rapidjson::Document JsonRequest::postRequest(rapidjson::Value& payload)
{
	auto alloc = mJsonDocument.GetAllocator();

	if (payload.IsObject()) {
		for (auto it = payload.MemberBegin(); it != payload.MemberEnd(); it++) {
			mJsonDocument.AddMember(it->name, it->value, alloc);
		}
	}
	auto uri = furi::uri_split::from_uri(mUrl);
	auto responseString = POST(uri.path.data(), mJsonDocument);
	auto responseJson = parseResponse(responseString);

	return responseJson;
}


std::string JsonRequest::POST(const char* path, const rapidjson::Document& payload)
{
	auto uri = furi::uri_split::from_uri(mUrl);
	// http | https
	std::string host = uri.scheme.data();
	// host:port
	host += "://" + std::string(uri.authority.data());
	httplib::Client cli(host);
		
	// set Content-Type Header to application/json
	httplib::Headers headers = {
		{"Content-Type", "application/json"},
		{"Accept", "*/*"}
	};
	if (mCookies.size()) {
		std::ostringstream cookie_stream;
		for (const auto& [key, value] : mCookies) {
			cookie_stream << key << "=" << value << ";";
		}
		std::string cookie_header = cookie_stream.str();
		cookie_header.pop_back(); // Entferne das letzte Semikolon

		// Füge den Cookie-Header hinzu
		headers.emplace("Cookie", cookie_header);
	}
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	payload.Accept(writer);
	auto res = cli.Post(path, headers, buffer.GetString(), buffer.GetSize(), "application/json");
	if (res->status != 200) {
		throw HttplibRequestException("status isn't 200 for POST", mUrl, res->status, magic_enum::enum_name(res.error()).data());
	}
	return res->body;
}

Document JsonRequest::parseResponse(std::string responseString)
{
	Document result;
	// extract parameter from request
	result.Parse(responseString.data());

	if (result.HasParseError()) {
		throw RapidjsonParseErrorException("error parsing request answer", result.GetParseError(), result.GetErrorOffset())
			.setRawText(responseString);
	}
	return result;
}