#include "ProtobufExceptions.h"
 
ProtobufSerializationException::ProtobufSerializationException(const google::protobuf::Message& message) noexcept
	: GradidoBlockchainException("Protobuf Serialization Exception"), mMessage(message)
{

}
std::string ProtobufSerializationException::getFullString() const noexcept
{
	return what();
}

// ****************** Protobuf parse exception **************************
ProtobufParseException::ProtobufParseException(const std::string& serializedProtobuf) noexcept
	: GradidoBlockchainException("Protobuf Parse from String Exception"), mSerializedProtobuf(serializedProtobuf)
{

}

std::string ProtobufParseException::getFullString() const noexcept
{
	std::string resultString;
	std::string serializedProtobufSize = std::to_string(mSerializedProtobuf.size());
	size_t resultSize = strlen(what()) + mSerializedProtobuf.size() + 17;
	resultString.reserve(resultSize);
	resultString = what();
	resultString += ", string size: " + serializedProtobufSize;
	return resultString;
}