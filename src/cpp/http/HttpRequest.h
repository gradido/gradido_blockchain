#ifndef __GRADIDO_POCO_HTTP_REQUEST
#define __GRADIDO_POCO_HTTP_REQUEST

#include <string>
#include "Poco/Net/NameValueCollection.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/URI.h"

#include "rapidjson/document.h"

#include "lib/NotificationList.h"

class HttpRequest : public NotificationList
{
public:
	HttpRequest(const std::string& url);
	HttpRequest(const std::string& host, int port, const char* path = nullptr, const char* query = nullptr);
	virtual ~HttpRequest() {}

protected:
	Poco::SharedPtr<Poco::Net::HTTPClientSession> createClientSession(NotificationList* errorReciver = nullptr);
	std::string GET(const char* pathAndQuery = nullptr, const char* version = nullptr);
	// 
	Poco::URI mRequestUri;

};

#endif //__GRADIDO_POCO_HTTP_REQUEST