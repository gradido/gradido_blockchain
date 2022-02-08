#ifndef __GRADIDO_BLOCKCHAIN_HTTP_REQUEST_EXCEPTION_H
#define __GRADIDO_BLOCKCHAIN_HTTP_REQUEST_EXCEPTION_H

#include "gradido_blockchain/GradidoBlockchainException.h"
#include "Poco/URI.h"

#include "rapidjson/error/error.h"
#include "rapidjson/document.h"

class RequestException : public GradidoBlockchainException
{
public:
	explicit RequestException(const char* what, const char* host, int port) noexcept;
	explicit RequestException(const char* what, const Poco::URI& uri) noexcept;
	std::string getFullString() const;

protected:
	Poco::URI mUri;
};

class RequestEmptyResponseException : public RequestException
{
public: 
	explicit RequestEmptyResponseException(const char* what, const char* host, int port) noexcept;
	explicit RequestEmptyResponseException(const char* what, const Poco::URI& uri) noexcept;
};

class RequestResponseInvalidJsonException : public RequestException
{
public:
	explicit RequestResponseInvalidJsonException(const char* what, const Poco::URI& uri, const std::string& rawText) noexcept;
	inline const std::string& getRawText() const { return mRawText; }

	//! write raw text to file with file name: <prefix><what><ending>
	//! used for debugging invalid json requests from server, especially from php based servers
	//! \return true if file could write, else false
	bool printToFile(const char* prefix, const char* ending = ".txt") const;

protected:
	std::string mRawText;
};



class RequestResponseErrorException : public RequestException
{
public:
	explicit RequestResponseErrorException(const char* what, const Poco::URI& uri, const std::string& msg) noexcept;

	RequestResponseErrorException& setDetails(const std::string& details);
	std::string getFullString() const;

protected:
	std::string mErrorMessage;
	std::string mErrorDetails;
};

class RequestResponseCakePHPException : public RequestResponseErrorException
{
public:
	explicit RequestResponseCakePHPException(const char* what, const Poco::URI& uri, const std::string& msg) noexcept;

	RequestResponseErrorException& setDetails(const rapidjson::Document& responseJson);
protected:

};

class HandleRequestException : public GradidoBlockchainException
{
public:
	HandleRequestException(const char* what) noexcept;
	std::string getFullString() const;
};


#endif //__GRADIDO_BLOCKCHAIN_HTTP_REQUEST_EXCEPTION_H
