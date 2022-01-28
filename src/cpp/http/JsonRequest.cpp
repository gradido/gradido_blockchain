
#include "JsonRequest.h"

#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "GradidoBlockchainException.h"

using namespace rapidjson;

JsonRequest::JsonRequest(const std::string& serverHost, int serverPort)
	: HttpRequest(serverHost, serverPort), mJsonDocument(kObjectType)
{

}

JsonRequest::~JsonRequest()
{

}

Document JsonRequest::postRequest(const char* path, Value& payload)
{
	static const char* functionName = "JsonRequest::requestLogin";

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


std::string JsonRequest::POST(const char* path, const rapidjson::Document& payload, const char* version/* = nullptr*/)
{
	auto clientSession = createClientSession();
	
	std::string _version = "HTTP/1.0";
	if (version) _version = version;
	Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, path, _version);

	request.setChunkedTransferEncoding(true);
	request.setContentType("application/json");
	request.add("Accept", "*/*");
	std::ostream& request_stream = clientSession->sendRequest(request);

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	payload.Accept(writer);
	request_stream << std::string(buffer.GetString(), buffer.GetSize());

	Poco::Net::HTTPResponse response;
	std::istream& response_stream = clientSession->receiveResponse(response);

	// debugging answer

	std::string responseString;
	for (std::string line; std::getline(response_stream, line); ) {
		responseString += line + "\n";
	}
	
	return responseString;
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