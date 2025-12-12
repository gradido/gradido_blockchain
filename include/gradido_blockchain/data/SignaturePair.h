#ifndef __GRADIDO_BLOCKCHAIN_DATA_SIGNATURE_PAIR_H
#define __GRADIDO_BLOCKCHAIN_DATA_SIGNATURE_PAIR_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/memory/Block.h"

namespace gradido {
	namespace data {
		class GRADIDOBLOCKCHAIN_EXPORT SignaturePair
		{
		public:
			SignaturePair();
			// throw InvalidSizeException
			SignaturePair(memory::ConstBlockPtr pubkeyPtr, memory::ConstBlockPtr signaturePtr);
			~SignaturePair();

			inline bool operator==(const SignaturePair& other) const;
			inline bool operator !=(const SignaturePair& other) const {
				return !(*this == other);
			}
			inline memory::ConstBlockPtr getPublicKey() const { return mPublicKey; }
			inline memory::ConstBlockPtr getSignature() const { return mSignature; }
			inline int64_t hash() const { return mHash; };
		protected:
			int64_t calculateHash() const;
			memory::ConstBlockPtr mPublicKey;
			memory::ConstBlockPtr mSignature;
			int64_t mHash;
		};

		bool SignaturePair::operator==(const SignaturePair& other) const {
			if (mHash != other.mHash) {
				return false;
			}
			return mPublicKey->isTheSame(other.mPublicKey) && mSignature->isTheSame(other.mSignature);
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_DATA_SIGNATURE_PAIR_H