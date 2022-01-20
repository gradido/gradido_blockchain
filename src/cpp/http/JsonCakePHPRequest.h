#ifndef __GRADIDO_POCO_HTTP_JSON_CAKE_PHP_REQUEST
#define __GRADIDO_POCO_HTTP_JSON_CAKE_PHP_REQUEST

#include "JsonRequest.h"

enum JsonRequestReturn
{
	JSON_REQUEST_RETURN_OK,
	JSON_REQUEST_RETURN_PARSE_ERROR,
	JSON_REQUEST_RETURN_ERROR,
	JSON_REQUEST_PARAMETER_ERROR,
	JSON_REQUEST_CONNECT_ERROR
};

class JsonCakePHPRequest : public JsonRequest
{
public:
	JsonCakePHPRequest(const std::string& serverHost, int serverPort);
	~JsonCakePHPRequest() {}

	JsonRequestReturn request(const char* methodName);
	JsonRequestReturn request(const char* methodName, rapidjson::Value& payload);

protected:
};

#endif //__GRADIDO_POCO_HTTP_JSON_CAKE_PHP_REQUEST