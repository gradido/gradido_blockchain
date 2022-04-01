#ifndef __GRADIDO_BLOCKCHAIN_CONNECTOR_CRYPTO_SEALED_BOXES
#define __GRADIDO_BLOCKCHAIN_CONNECTOR_CRYPTO_SEALED_BOXES

#include "gradido_blockchain/crypto/AuthenticatedEncryption.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

namespace SealedBoxes
{
	MemoryBin* encrypt(const AuthenticatedEncryption* keys, const std::string& message);
	std::string decrypt(const AuthenticatedEncryption* keys, const MemoryBin* encryptedMessage);

	class DecryptException : public GradidoBlockchainException
	{
	public:
		explicit DecryptException(const char* what) noexcept
			: GradidoBlockchainException(what) {}

		std::string getFullString() const { return what(); }

	};
}

#endif //__GRADIDO_BLOCKCHAIN_CONNECTOR_CRYPTO_SEALED_BOXES