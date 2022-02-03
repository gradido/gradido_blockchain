#ifndef __GRADIDO_BLOCKCHAIN_EXCEPTION_H
#define __GRADIDO_BLOCKCHAIN_EXCEPTION_H

#include <stdexcept>

#include "rapidjson/error/error.h"

class GradidoBlockchainException : public std::runtime_error
{
public:
	explicit GradidoBlockchainException(const char* what) : std::runtime_error(what) {}
	virtual ~GradidoBlockchainException() {};
	virtual std::string getFullString() const = 0;
};

class RapidjsonParseErrorException : public GradidoBlockchainException
{
public:
	explicit RapidjsonParseErrorException(const char* what, rapidjson::ParseErrorCode parseErrorCode, size_t parseErrorOffset) noexcept;

	RapidjsonParseErrorException& setRawText(const std::string& rawText);
	std::string getFullString() const;

protected:
	rapidjson::ParseErrorCode mParseErrorCode;
	size_t					  mParseErrorOffset;
	std::string				  mRawText;
};

class GradidoInvalidEnumException : public GradidoBlockchainException
{
public:
	explicit GradidoInvalidEnumException(const char* what, const std::string& enumString) noexcept;
	std::string getFullString() const;

protected:
	std::string mEnumString;
};

#endif __GRADIDO_BLOCKCHAIN_EXCEPTION_H