#ifndef __GRADIDO_BLOCKCHAIN_DATA_PUBLIC_KEY_H
#define __GRADIDO_BLOCKCHAIN_DATA_PUBLIC_KEY_H

#include <array>
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

#define ED25519_PUBLIC_KEY_SIZE 32U

namespace gradido {
	namespace data {
		class GRADIDOBLOCKCHAIN_EXPORT PublicKey 
		{
		public:
			// throw InvalidPublicKeySizeException
			PublicKey(memory::ConstMemoryBlockPtr pubkeyPtr);

			bool operator ==(const PublicKey& other) const { return publicKey == other.publicKey; }
			bool operator !=(const PublicKey& other) const { return publicKey != other.publicKey; }

			static size_t size() { return ED25519_PUBLIC_KEY_SIZE; }
		protected:
			std::array<uint8_t, ED25519_PUBLIC_KEY_SIZE> publicKey;
		};

	}
}

#endif //__GRADIDO_BLOCKCHAIN_DATA_PUBLIC_KEY_H
