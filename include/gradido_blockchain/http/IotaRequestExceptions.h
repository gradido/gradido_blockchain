#ifndef __GRADIDO_BLOCKCHAIN_HTTP_IOTA_REQUEST_EXCEPTIONS_H
#define __GRADIDO_BLOCKCHAIN_HTTP_IOTA_REQUEST_EXCEPTIONS_H

#include "gradido_blockchain/GradidoBlockchainException.h"
#include "rapidjson/document.h"

class GRADIDOBLOCKCHAIN_EXPORT IotaRequestException : public GradidoBlockchainException
{
public: 
	explicit IotaRequestException(const char* what, const std::string& url) noexcept;
	std::string getFullString() const;
protected:
	std::string mUrl;

};

class GRADIDOBLOCKCHAIN_EXPORT IotaPostRequestException : public IotaRequestException
{
public:
	explicit IotaPostRequestException(const char* what, const std::string& url) noexcept;
	std::string getFullString() const;
	IotaPostRequestException& setRequestJson(const rapidjson::Document& requestJson);

protected:
	std::string mRequestJsonPrettyString;
};

#endif //__GRADIDO_BLOCKCHAIN_HTTP_IOTA_REQUEST_EXCEPTIONS_H