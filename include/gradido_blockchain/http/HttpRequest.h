#ifndef __GRADIDO_POCO_HTTP_REQUEST
#define __GRADIDO_POCO_HTTP_REQUEST

#include "gradido_blockchain/export.h"

#include "rapidjson/document.h"

#include <string>

class GRADIDOBLOCKCHAIN_EXPORT HttpRequest
{
public:
	HttpRequest(const std::string& url);
	HttpRequest(const std::string& host, int port, const char* path = nullptr, const char* query = nullptr);
	virtual ~HttpRequest() {}

protected:
	std::string mUrl;
	std::string GET(const std::string& _pathAndQuery);	
};

#endif //__GRADIDO_POCO_HTTP_REQUEST