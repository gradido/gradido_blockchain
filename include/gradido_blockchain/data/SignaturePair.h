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
				: mPubkey(pubkeyPtr), mSignature(signaturePtr) {}

			inline bool operator==(const SignaturePair& other) const {
				return mPubkey->isTheSame(other.mPubkey) && mSignature->isTheSame(other.mSignature);
			}
			inline memory::ConstBlockPtr getPubkey() const { return mPubkey; }
			inline memory::ConstBlockPtr getSignature() const { return mSignature; }
		protected:
			memory::ConstBlockPtr mPubkey;
			memory::ConstBlockPtr mSignature;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_DATA_SIGNATURE_PAIR_H