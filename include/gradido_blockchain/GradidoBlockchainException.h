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

class GRADIDOBLOCKCHAIN_EXPORT GradidoBlockchainTransactionAlreadyExistException : public GradidoBlockchainException
{
public:
	explicit GradidoBlockchainTransactionAlreadyExistException(const char* what) noexcept;
	std::string getFullString() const;
	inline GradidoBlockchainTransactionAlreadyExistException& setTransactionId(uint64_t id) { mTransactionId = id; return *this; }
protected:
	uint64_t mTransactionId;
};

class GRADIDOBLOCKCHAIN_EXPORT RapidjsonParseErrorException : public GradidoBlockchainException
{
public:
	explicit RapidjsonParseErrorException(const char* what, rapidjson::ParseErrorCode parseErrorCode, size_t parseErrorOffset) noexcept;

	RapidjsonParseErrorException& setRawText(const std::string& rawText);
	inline const std::string& getRawText() { return mRawText; }
	std::string getFullString() const;
	std::string getDetails() const;

protected:
	rapidjson::ParseErrorCode mParseErrorCode;
	size_t					  mParseErrorOffset;
	std::string				  mRawText;
};

class GRADIDOBLOCKCHAIN_EXPORT RapidjsonMissingMemberException : public GradidoBlockchainException
{
public: 
	explicit RapidjsonMissingMemberException(const char* what, const char* fieldName, const char* fieldType) noexcept;

	std::string getFullString() const;
protected:
	std::string mFieldName;
	std::string mFieldType;

};

class GRADIDOBLOCKCHAIN_EXPORT RapidjsonInvalidMemberException : public RapidjsonMissingMemberException
{
public:
	explicit RapidjsonInvalidMemberException(const char* what, const char* fieldName, const char* fieldType) noexcept :
		RapidjsonMissingMemberException(what, fieldName, fieldType) {}
};

class GRADIDOBLOCKCHAIN_EXPORT GradidoInvalidEnumException : public GradidoBlockchainException
{
public:
	explicit GradidoInvalidEnumException(const char* what, const std::string& enumString) noexcept;
	std::string getFullString() const;

protected:
	std::string mEnumString;
};

class GRADIDOBLOCKCHAIN_EXPORT GradidoUnknownEnumException : public GradidoBlockchainException
{
public:
	explicit GradidoUnknownEnumException(const char* what, const char* enumName, int value) noexcept;
	std::string getFullString() const;

protected:
	std::string mEnumName;
	int mValue;
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

class GRADIDOBLOCKCHAIN_EXPORT BlockchainOrderException : public GradidoBlockchainException
{
public:
	explicit BlockchainOrderException(const char* what) noexcept;
	std::string getFullString() const;
};

namespace model {
	namespace gradido {
		enum TransactionType: uint8_t;
		class TransactionBase;
	}
}

class GRADIDOBLOCKCHAIN_EXPORT InvalidTransactionTypeOnBlockchain : public GradidoBlockchainException
{
public:
	explicit InvalidTransactionTypeOnBlockchain(const char* what, model::gradido::TransactionType type) noexcept;

	std::string getFullString() const;

protected:
	model::gradido::TransactionType mTransactionType;
};

class GRADIDOBLOCKCHAIN_EXPORT InvalidCrossGroupTransaction : public InvalidTransactionTypeOnBlockchain
{
public:
	explicit InvalidCrossGroupTransaction(const char* what, model::gradido::TransactionType type) noexcept
		: InvalidTransactionTypeOnBlockchain(what, type) {}
};

#endif //__GRADIDO_BLOCKCHAIN_EXCEPTION_H