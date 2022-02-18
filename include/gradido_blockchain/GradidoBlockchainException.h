#ifndef __GRADIDO_BLOCKCHAIN_EXCEPTION_H
#define __GRADIDO_BLOCKCHAIN_EXCEPTION_H

#include <stdexcept>
#include "gradido_blockchain/export.h"
#include "rapidjson/error/error.h"

class GRADIDOBLOCKCHAIN_EXPORT GradidoBlockchainException : public std::runtime_error
{
public:
	explicit GradidoBlockchainException(const char* what) : std::runtime_error(what) {}
	virtual ~GradidoBlockchainException() {};
	virtual std::string getFullString() const = 0;
};

class GRADIDOBLOCKCHAIN_EXPORT GradidoBlockchainTransactionNotFoundException : public GradidoBlockchainException
{
public:
	explicit GradidoBlockchainTransactionNotFoundException(const char* what) noexcept;
	std::string getFullString() const;
	inline GradidoBlockchainTransactionNotFoundException& setTransactionId(uint64_t id) { mTransactionId = id; return *this; }
protected:
	uint64_t mTransactionId;
};

class GRADIDOBLOCKCHAIN_EXPORT RapidjsonParseErrorException : public GradidoBlockchainException
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

class GRADIDOBLOCKCHAIN_EXPORT GradidoInvalidEnumException : public GradidoBlockchainException
{
public:
	explicit GradidoInvalidEnumException(const char* what, const std::string& enumString) noexcept;
	std::string getFullString() const;

protected:
	std::string mEnumString;
};

class GRADIDOBLOCKCHAIN_EXPORT GradidoInvalidBase64Exception : public GradidoBlockchainException
{
public:
	explicit GradidoInvalidBase64Exception(const char* what, const std::string& base64, int lastValidCharacter) noexcept;
	std::string getFullString() const;
protected:
	std::string mBase64;
	int         mLastValidCharacter;

};


#endif __GRADIDO_BLOCKCHAIN_EXCEPTION_H