#ifndef __GRADIDO_BLOCKCHAIN_DATA_SIGNATURE_H
#define __GRADIDO_BLOCKCHAIN_DATA_SIGNATURE_H

#include <array>
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

namespace gradido {
	namespace data {
#define ED25519_SIGNATURE_SIZE 64U

		class GRADIDOBLOCKCHAIN_EXPORT Signature
		{
		public:
			// throw InvalidPublicKeySizeException
			Signature(memory::ConstMemoryBlockPtr signaturePtr);

			bool operator ==(const Signature& other) const { return signature == other.signature; }
			bool operator !=(const Signature& other) const { return signature != other.signature; }

			static size_t size() { return ED25519_SIGNATURE_SIZE; }
		protected:
			std::array<uint8_t, ED25519_SIGNATURE_SIZE> signature;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_DATA_SIGNATURE_H