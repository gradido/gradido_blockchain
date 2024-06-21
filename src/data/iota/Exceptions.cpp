#include "gradido_blockchain/data/iota/Exceptions.h"

namespace iota {

	MessageIdFormatException::MessageIdFormatException(const char* what, const memory::Block& messageId) noexcept
		: GradidoBlockchainException(what), mMessageIdHex(messageId.convertToHex())
	{
	}
	MessageIdFormatException::MessageIdFormatException(const char* what, std::string_view messageIdHex) noexcept
		: GradidoBlockchainException(what), mMessageIdHex(messageIdHex)
	{

	}

	std::string MessageIdFormatException::getFullString() const
	{
		std::string resultString;
		size_t resultSize = strlen(what()) + 22 + 2 + mMessageIdHex.size();
		resultString.reserve(resultSize);
		resultString = what();
		resultString += " with message id hex: " + mMessageIdHex;
		return resultString;
	}
}
