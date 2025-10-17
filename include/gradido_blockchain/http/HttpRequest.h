#ifndef __GRADIDO_POCO_HTTP_REQUEST
#define __GRADIDO_POCO_HTTP_REQUEST

#include "gradido_blockchain/export.h"

#include <string>
#include <map>

class GRADIDOBLOCKCHAIN_EXPORT HttpRequest
{
public:
	HttpRequest(const std::string& url);
	HttpRequest(const std::string& host, int port, const char* path = nullptr, const char* query = nullptr);
	virtual ~HttpRequest() {}

	// don't use for big requests
	std::string POST(const std::string& body, const char* contentType = "application/json", const char* path = nullptr);
	// don't use for big requests
	std::string  GET(const std::map<std::string, std::string> query, const char* path = nullptr);
	std::string  GET(const char* path);

	void addCookie(const std::string& name, const std::string& value) { mCookies.insert({ name, value }); }
	void addHeader(const std::string& name, const std::string& value) { mHeaders.insert({ name, value }); }

protected:
	std::string constructHostString();

	std::string mUrl;
	std::multimap<std::string, std::string> mCookies;
	std::multimap<std::string, std::string> mHeaders;
	bool mIsSSL;
};

#endif //__GRADIDO_POCO_HTTP_REQUEST