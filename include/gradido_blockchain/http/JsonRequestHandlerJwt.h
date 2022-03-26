#ifndef __GRADIDO_BLOCKCHAIN_HTTP_JSON_REQUEST_HANDLER_JWT_h
#define __GRADIDO_BLOCKCHAIN_HTTP_JSON_REQUEST_HANDLER_JWT_h

#include "JsonRequestHandler.h"

class JsonRequestHandlerJwt : public JsonRequestHandler
{
public:
	JsonRequestHandlerJwt(Poco::Net::IPAddress clientIp);

	void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
};

#endif //__GRADIDO_BLOCKCHAIN_HTTP_JSON_REQUEST_HANDLER_JWT_h