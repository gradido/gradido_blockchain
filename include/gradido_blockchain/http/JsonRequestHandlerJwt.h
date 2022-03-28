#ifndef __GRADIDO_BLOCKCHAIN_HTTP_JSON_REQUEST_HANDLER_JWT_h
#define __GRADIDO_BLOCKCHAIN_HTTP_JSON_REQUEST_HANDLER_JWT_h

#include "JsonRequestHandler.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "Poco/JWT/Token.h"

class JsonRequestHandlerJwt : public JsonRequestHandler
{
public:
	JsonRequestHandlerJwt();
	JsonRequestHandlerJwt(Poco::Net::IPAddress clientIp);

	virtual ~JsonRequestHandlerJwt();

	void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);

	inline const Poco::JWT::Token* getJwtToken() { return mJWTToken; }

protected:
	Poco::JWT::Token* mJWTToken;
};

class JwtTokenException : public GradidoBlockchainException
{
public:
	explicit JwtTokenException(const char* what, const Poco::JWT::Token* jwtToken) noexcept;

	std::string getFullString() const;

protected:
	Poco::JWT::Token mJwtToken;
};

#endif //__GRADIDO_BLOCKCHAIN_HTTP_JSON_REQUEST_HANDLER_JWT_h