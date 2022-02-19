#ifndef __GRADIDO_POCO_HTTP_JSON_CAKE_PHP_REQUEST
#define __GRADIDO_POCO_HTTP_JSON_CAKE_PHP_REQUEST

#include "JsonRequest.h"

class GRADIDOBLOCKCHAIN_EXPORT JsonCakePHPRequest : public JsonRequest
{
public:
	JsonCakePHPRequest(const std::string& serverHost, int serverPort);
	~JsonCakePHPRequest() {}

	bool request(const char* methodName);
	bool request(const char* methodName, rapidjson::Value& payload);

protected:
};

#endif //__GRADIDO_POCO_HTTP_JSON_CAKE_PHP_REQUEST