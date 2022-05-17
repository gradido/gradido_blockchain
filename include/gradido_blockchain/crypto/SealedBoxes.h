#ifndef __GRADIDO_BLOCKCHAIN_CONNECTOR_CRYPTO_SEALED_BOXES
#define __GRADIDO_BLOCKCHAIN_CONNECTOR_CRYPTO_SEALED_BOXES

#include "gradido_blockchain/crypto/AuthenticatedEncryption.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

namespace SealedBoxes
{
	GRADIDOBLOCKCHAIN_EXPORT MemoryBin* encrypt(const AuthenticatedEncryption* keys, const std::string& message);
	GRADIDOBLOCKCHAIN_EXPORT std::string decrypt(const AuthenticatedEncryption* keys, const MemoryBin* encryptedMessage);
	GRADIDOBLOCKCHAIN_EXPORT std::string decrypt(const MemoryBin* privateKey, const MemoryBin* encryptedMessage);

	class GRADIDOBLOCKCHAIN_EXPORT DecryptException : public GradidoBlockchainException
	{
	public:
		explicit DecryptException(const char* what) noexcept
			: GradidoBlockchainException(what) {}

		std::string getFullString() const { return what(); }

	};
}

#endif //__GRADIDO_BLOCKCHAIN_CONNECTOR_CRYPTO_SEALED_BOXES
