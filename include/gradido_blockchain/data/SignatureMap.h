#ifndef __GRADIDO_BLOCKCHAIN_DATA_SIGNATURE_MAP_H
#define __GRADIDO_BLOCKCHAIN_DATA_SIGNATURE_MAP_H

#include "SignaturePair.h"

namespace gradido {
	namespace data {
		class GRADIDOBLOCKCHAIN_EXPORT SignatureMap
		{
		public:
			SignatureMap(size_t sizeHint = 1) { mSignaturePairs.reserve(sizeHint); }
			SignatureMap(SignaturePair firstSignaturePair, size_t sizeHint = 1): SignatureMap(sizeHint) {
				push(firstSignaturePair);
			}

			//! attention! not thread safe
			void push(const SignaturePair& signaturePair);
			inline void clear() { mSignaturePairs.clear(); }
			inline const std::vector<SignaturePair>& getSignaturePairs() const { return mSignaturePairs; }

		protected:
			std::vector<SignaturePair> mSignaturePairs;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_DATA_SIGNATURE_MAP_H