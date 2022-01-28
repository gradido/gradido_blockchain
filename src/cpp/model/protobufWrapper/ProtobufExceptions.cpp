#include "ProtobufExceptions.h"
 
ProtobufSerializationException::ProtobufSerializationException(const google::protobuf::Message& message) noexcept
	: GradidoBlockchainException("Protobuf Serialization Exception"), mMessage(message)
{

}
std::string ProtobufSerializationException::getFullString() const noexcept
{
	return what();
}