#ifndef __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_PROTOBUF_EXCEPTIONS_
#define __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_PROTOBUF_EXCEPTIONS_

#include "GradidoBlockchainException.h"
#include <google/protobuf/message.h>

class ProtobufSerializationException : public GradidoBlockchainException
{
public:
	explicit ProtobufSerializationException(const google::protobuf::Message& message) noexcept;
	std::string getFullString() const noexcept;
protected:

	const google::protobuf::Message& mMessage;

};

#endif //__GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_PROTOBUF_EXCEPTIONS_