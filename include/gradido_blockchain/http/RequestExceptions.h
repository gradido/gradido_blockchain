#ifndef __GRADIDO_BLOCKCHAIN_HTTP_REQUEST_EXCEPTION_H
#define __GRADIDO_BLOCKCHAIN_HTTP_REQUEST_EXCEPTION_H

#include "gradido_blockchain/GradidoBlockchainException.h"
#include "JsonRPCRequest.h"

#include "rapidjson/error/error.h"
#include "rapidjson/document.h"

class GRADIDOBLOCKCHAIN_EXPORT RequestException : public GradidoBlockchainException
{
public:
	explicit RequestException(const char* what, const char* host, int port) noexcept;
	explicit RequestException(const char* what, const std::string& uri) noexcept;
	std::string getFullString() const;

protected:
	std::string mUri;
};

class GRADIDOBLOCKCHAIN_EXPORT RequestEmptyResponseException : public RequestException
{
public: 
	explicit RequestEmptyResponseException(const char* what, const char* host, int port) noexcept;
	explicit RequestEmptyResponseException(const char* what, const std::string& uri) noexcept;
};

class GRADIDOBLOCKCHAIN_EXPORT RequestResponseInvalidJsonException : public RequestException
{
public:
	explicit RequestResponseInvalidJsonException(const char* what, const std::string& uri, const std::string& rawText) noexcept;
	inline const std::string& getRawText() const { return mRawText; }

	//! write raw text to file with file name: <prefix><what><ending>
	//! used for debugging invalid json requests from server, especially from php based servers
	//! \return true if file could write, else false
	bool printToFile(const char* prefix, const char* ending = ".txt") const;
	bool containRawHtmlClosingTag() const;

protected:
	std::string mRawText;
};

class GRADIDOBLOCKCHAIN_EXPORT RequestResponseErrorException : public RequestException
{
public:
	explicit RequestResponseErrorException(const char* what, const std::string& uri, const std::string& msg) noexcept;

	rapidjson::Value getDetails(rapidjson::Document::AllocatorType& alloc) const;
	RequestResponseErrorException& setDetails(const std::string& details);
	RequestResponseErrorException& setDetails(const rapidjson::Value& details);
	std::string getFullString() const;

protected:
	std::string mErrorMessage;
	std::string mErrorDetails;
};

class GRADIDOBLOCKCHAIN_EXPORT HttplibRequestException : public RequestException
{
public:
	explicit HttplibRequestException(const char* what, const std::string& uri, int status, const char* error) noexcept;
	virtual ~HttplibRequestException() {};
	std::string getFullString() const;

protected:
	int mStatus;
	std::string mError;
	
};

class GRADIDOBLOCKCHAIN_EXPORT RequestResponseCakePHPException : public RequestResponseErrorException
{
public:
	explicit RequestResponseCakePHPException(const char* what, const std::string& uri, const std::string& msg) noexcept;

	RequestResponseErrorException& setDetails(const rapidjson::Document& responseJson);
protected:

};

class GRADIDOBLOCKCHAIN_EXPORT HandleRequestException : public GradidoBlockchainException
{
public:
	explicit HandleRequestException(const char* what) noexcept;
	std::string getFullString() const;
};


class GRADIDOBLOCKCHAIN_EXPORT JsonRPCException : public GradidoBlockchainException
{
public:
	explicit JsonRPCException(const char* what, JsonRPCErrorCodes errorCode) noexcept;
	std::string getFullString() const;
	inline JsonRPCErrorCodes getErrorCode() const { return mErrorCode; }

protected:
	JsonRPCErrorCodes mErrorCode;
};

#endif //__GRADIDO_BLOCKCHAIN_HTTP_REQUEST_EXCEPTION_H
