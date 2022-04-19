#include "gradido_blockchain/model/protobufWrapper/ProtobufExceptions.h"
#include "gradido_blockchain/lib/DataTypeConverter.h" 

ProtobufSerializationException::ProtobufSerializationException(const char* what, const google::protobuf::Message& message) noexcept
	: GradidoBlockchainException(what), mMessage(message)
{

}

ProtobufSerializationException::ProtobufSerializationException(const google::protobuf::Message& message) noexcept
	: GradidoBlockchainException("Protobuf Serialization Exception"), mMessage(message)
{

}
std::string ProtobufSerializationException::getFullString() const noexcept
{
	return what();
}

// ****************** Protobuf serialize to json exception **************
ProtobufJsonSerializationException::ProtobufJsonSerializationException(const char* what, const google::protobuf::Message& message, google::protobuf::util::status_internal::Status status) noexcept
	: ProtobufSerializationException(what, message), mStatus(status)
{

}

std::string ProtobufJsonSerializationException::getFullString() const noexcept
{
	std::string resultString;
	std::string statusString = mStatus.ToString();
	size_t resultSize = strlen(what()) + statusString.size() + 16;
	resultString.reserve(resultSize);
	resultString = what();
	resultString += " with status: " + statusString;
	return resultString;
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

std::string ProtobufParseException::getSerializedAsBase64()
{
	return DataTypeConverter::binToBase64(mSerializedProtobuf);
}