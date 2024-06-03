#include "gradido_blockchain/data/Signature.h"
#include <string>

namespace gradido {
	namespace data {

		Signature::Signature(memory::ConstMemoryBlockPtr signaturePtr)
		{
			if (signaturePtr->size() != ED25519_SIGNATURE_SIZE) {
				throw InvalidSizeException("invalid signature in constructor", ED25519_SIGNATURE_SIZE, signaturePtr->size());
			}
			memcpy(signature.data(), *signaturePtr, ED25519_SIGNATURE_SIZE);
		}
	}
}