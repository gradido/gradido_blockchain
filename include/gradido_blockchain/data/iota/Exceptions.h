#ifndef __GRADIDO_BLOCKCHAIN_DATA_IOTA_EXCEPTIONS_H
#define __GRADIDO_BLOCKCHAIN_DATA_IOTA_EXCEPTIONS_H

#include "gradido_blockchain/GradidoBlockchainException.h"

namespace iota {

	class MessageIdFormatException : public GradidoBlockchainException
	{
	public:
		explicit MessageIdFormatException(const char* what, const memory::Block& messageId) noexcept;
		explicit MessageIdFormatException(const char* what, std::string_view messageIdHex) noexcept;
		std::string getFullString() const;

	protected:
		std::string mMessageIdHex;
	};

}


#endif //__GRADIDO_BLOCKCHAIN_DATA_IOTA_EXCEPTIONS_H