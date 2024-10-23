#ifndef __GRADIDO_BLOCKCHAIN_HTTP_ABSTRACT_RESPONSE_HANDLER_H
#define __GRADIDO_BLOCKCHAIN_HTTP_ABSTRACT_RESPONSE_HANDLER_H

#include "MethodType.h"

namespace httplib {
	class Request;
	class Response;
}

class AbstractResponseHandler 
{
public:
	virtual ~AbstractResponseHandler() {};
	virtual void handleRequest(const httplib::Request& request, httplib::Response& response, MethodType method) = 0;
};

#endif // __GRADIDO_BLOCKCHAIN_HTTP_ABSTRACT_RESPONSE_HANDLER_H