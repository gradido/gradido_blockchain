#ifndef __GRADIDO_BLOCKCHAIN_HTTP_IOTA_REQUEST_EXCEPTIONS_H
#define __GRADIDO_BLOCKCHAIN_HTTP_IOTA_REQUEST_EXCEPTIONS_H

#include "GradidoBlockchainException.h"
#include "rapidjson/document.h"

class IotaRequestException : public GradidoBlockchainException
{
public: 
	explicit IotaRequestException(const char* what, const std::string& url) noexcept;
	std::string getFullString() const;
protected:
	std::string mUrl;

};

class IotaPostRequestException : public IotaRequestException
{
public:
	explicit IotaPostRequestException(const char* what, const std::string& url) noexcept;
	std::string getFullString() const;
	IotaPostRequestException& setRequestJson(const rapidjson::Document& requestJson);

protected:
	std::string mRequestJsonPrettyString;
};

#endif //__GRADIDO_BLOCKCHAIN_HTTP_IOTA_REQUEST_EXCEPTIONS_H