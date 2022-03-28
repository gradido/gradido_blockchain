#ifndef __GRADIDO_BLOCKCHAIN_HTTP_JSON_REQUEST_HANDLER_JWT_h
#define __GRADIDO_BLOCKCHAIN_HTTP_JSON_REQUEST_HANDLER_JWT_h

#include "JsonRequestHandler.h"
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

#endif //__GRADIDO_BLOCKCHAIN_HTTP_JSON_REQUEST_HANDLER_JWT_h