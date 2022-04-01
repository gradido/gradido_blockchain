#include "gradido_blockchain/http/JsonRequestHandlerJwt.h"

#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/JWT/JWT.h"
#include "Poco/JWT/Signer.h"
#include "Poco/JWT/Token.h"
#include "Poco/JSON/Stringifier.h"

#include "rapidjson/writer.h"

using namespace rapidjson;

JsonRequestHandlerJwt::JsonRequestHandlerJwt(Poco::Net::IPAddress clientIp)
	: JsonRequestHandler(clientIp)
{

}

JsonRequestHandlerJwt::JsonRequestHandlerJwt()
{

}

JsonRequestHandlerJwt::~JsonRequestHandlerJwt()
{

}

void JsonRequestHandlerJwt::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
	auto method = request.getMethod();
	printf("[JsonRequestHandlerJwt::handleRequest] method: %s\n", method.data());
	if (method == "OPTIONS") {
		responseOptions(response);
		return;
	}
	auto authorization = request.find("Authorization");
	if (authorization == request.end()) {
		responseWithJson(stateError("Authorization not found"), request, response);
		return;
	}
	auto startBearer = authorization->second.find("Bearer");
	if (authorization->second.find("Bearer") == std::string::npos) {
		responseWithJson(stateError("Authorization invalid format"), request, response);
		return;
	}
	mSerializedJWTToken = authorization->second.substr(startBearer + 7);

	JsonRequestHandler::handleRequest(request, response);	
}



JwtTokenException::JwtTokenException(const char* what, const Poco::JWT::Token* jwtToken) noexcept
	: GradidoBlockchainException(what), mJwtToken(jwtToken->toString())
{

}

JwtTokenException::JwtTokenException(const char* what, const std::string& serializedJwtToken) noexcept
	: GradidoBlockchainException(what), mJwtToken(serializedJwtToken)
{

}

std::string JwtTokenException::getFullString() const
{
	std::string resultString(what());
	std::stringstream ss;
	Poco::JSON::Stringifier::stringify(mJwtToken.payload(), ss);
	resultString += ", jwt payload: " + ss.str();
	return resultString;
}
