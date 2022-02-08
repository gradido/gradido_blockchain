#ifndef __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_PROTOBUF_EXCEPTIONS_
#define __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_PROTOBUF_EXCEPTIONS_

#include "gradido_blockchain/GradidoBlockchainException.h"
#include <google/protobuf/message.h>
#include <google/protobuf/util/json_util.h>

class ProtobufSerializationException : public GradidoBlockchainException
{
public:
	explicit ProtobufSerializationException(const char* what, const google::protobuf::Message& message) noexcept;
	explicit ProtobufSerializationException(const google::protobuf::Message& message) noexcept;
	std::string getFullString() const noexcept;
protected:
	const google::protobuf::Message& mMessage;
	
};

class ProtobufJsonSerializationException : public ProtobufSerializationException
{
public: 
	explicit ProtobufJsonSerializationException(const char* what, const google::protobuf::Message& message, google::protobuf::util::status_internal::Status status) noexcept;
	std::string getFullString() const noexcept;
protected:
	google::protobuf::util::status_internal::Status mStatus;
};

class ProtobufParseException : public GradidoBlockchainException
{
public:
	explicit ProtobufParseException(const std::string& serializedProtobuf) noexcept;
	std::string getFullString() const noexcept;

protected:
	std::string mSerializedProtobuf;
	
};

#endif //__GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_PROTOBUF_EXCEPTIONS_