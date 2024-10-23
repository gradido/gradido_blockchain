#ifndef __GRADIDO_BLOCKCHAIN_HTTP_ABSTRACT_RESPONSE_HANDLER_FACTORY_H
#define __GRADIDO_BLOCKCHAIN_HTTP_ABSTRACT_RESPONSE_HANDLER_FACTORY_H

#include "AbstractResponseHandler.h"
#include <memory>

class AbstractResponseHandlerFactory
{
public: 
	//! \return response hander for method
	virtual std::unique_ptr<AbstractResponseHandler> getResponseHandler(MethodType method) = 0;
	//! \return check if factory has a response handler for a specific method, else server didn't need to listen for requests
	virtual bool has(MethodType method) = 0;
};

#endif // __GRADIDO_BLOCKCHAIN_HTTP_ABSTRACT_RESPONSE_HANDLER_FACTORY_H