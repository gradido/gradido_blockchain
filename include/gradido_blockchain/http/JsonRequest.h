/*!
 *
 * \author: Dario Rekowski
 *
 * \date: 13.12.2019
 *
 * \brief: Class for Json Requests to php server
 *
*/

#ifndef __GRADIDO_LOGIN_SERVER_LIB_JSON_REQUEST_
#define __GRADIDO_LOGIN_SERVER_LIB_JSON_REQUEST_

#include "HttpRequest.h"


class GRADIDOBLOCKCHAIN_EXPORT JsonRequest : public HttpRequest
{
public:
	JsonRequest(const std::string& serverHost, int serverPort);
	JsonRequest(const Poco::URI& requestUri);
	virtual ~JsonRequest();

	rapidjson::Document postRequest(const char* path, rapidjson::Value& payload);
	rapidjson::Document getRequest(const char* path);
	//! \brief take path from mRequestURI
	rapidjson::Document postRequest(rapidjson::Value& payload);

	rapidjson::Document::AllocatorType& getJsonAllocator() { return mJsonDocument.GetAllocator(); }

	void addCookie(const std::string& name, const std::string& value) { mCookies.add(name, value); }

protected:
	std::string POST(const char* path, const rapidjson::Document& payload, const char* version = nullptr);
	rapidjson::Document parseResponse(std::string responseString);

	rapidjson::Document mJsonDocument;
	Poco::Net::NameValueCollection mCookies;
};


#endif //__GRADIDO_LOGIN_SERVER_LIB_JSON_REQUEST_