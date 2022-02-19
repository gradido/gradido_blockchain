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
	virtual ~JsonRequest();

	rapidjson::Document postRequest(const char* path, rapidjson::Value& payload);

	rapidjson::Document::AllocatorType& getJsonAllocator() { return mJsonDocument.GetAllocator(); }

protected:
	std::string POST(const char* path, const rapidjson::Document& payload, const char* version = nullptr);
	rapidjson::Document parseResponse(std::string responseString);

	rapidjson::Document mJsonDocument;
};


#endif //__GRADIDO_LOGIN_SERVER_LIB_JSON_REQUEST_