#ifndef __GRADIDO_LOGIN_SERVER_IOTA_REQUEST
#define __GRADIDO_LOGIN_SERVER_IOTA_REQUEST

#include "JsonRequest.h"

#include "rapidjson/document.h"

class IotaRequest : public JsonRequest
{
public:
	IotaRequest(const std::string& serverHost, int serverPort, const std::string& urlPath);
	~IotaRequest();	

	//! \return messageId as hex string
	std::string sendMessage(const std::string& indexHex, const std::string& messageHex, NotificationList* errorReciver);

protected:
	// Iota get parent message ids for own message
	std::vector<std::string> getTips(NotificationList* errorReciver);

};

#endif //__GRADIDO_LOGIN_SERVER_IOTA_REQUEST