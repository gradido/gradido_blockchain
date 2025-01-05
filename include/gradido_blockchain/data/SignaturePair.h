#ifndef __GRADIDO_BLOCKCHAIN_DATA_SIGNATURE_PAIR_H
#define __GRADIDO_BLOCKCHAIN_DATA_SIGNATURE_PAIR_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/memory/Block.h"

namespace gradido {
	namespace data {
		class GRADIDOBLOCKCHAIN_EXPORT SignaturePair
		{
		public:
			SignaturePair() {}
			// throw InvalidSizeException
			SignaturePair(memory::ConstBlockPtr pubkeyPtr, memory::ConstBlockPtr signaturePtr)
				: mPublicKey(pubkeyPtr), mSignature(signaturePtr) {}

			inline bool operator==(const SignaturePair& other) const {
				return mPublicKey->isTheSame(other.mPublicKey) && mSignature->isTheSame(other.mSignature);
			}
			inline memory::ConstBlockPtr getPublicKey() const { return mPublicKey; }
			inline memory::ConstBlockPtr getSignature() const { return mSignature; }
		protected:
			memory::ConstBlockPtr mPublicKey;
			memory::ConstBlockPtr mSignature;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_DATA_SIGNATURE_PAIR_H