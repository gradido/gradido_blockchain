#ifndef __GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_SIGNATURE_MAP_H
#define __GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_SIGNATURE_MAP_H

#include "SignaturePair.h"

namespace model {
	namespace protopuf {
		
		using SignatureMapMessage = message<
			message_field<"sig_pair", 1, SignaturePairMessage, repeated>
		>;
		
		class GRADIDOBLOCKCHAIN_EXPORT SignatureMap {
		public:
			SignatureMap() {}
			SignatureMap(const SignatureMapMessage& data);
			SignatureMap(SignaturePair firstSignaturePair);
			~SignatureMap() {}

			inline void addSignaturePair(SignaturePair signaturePair) { mSignaturePairs.push_back(std::move(signaturePair));}
			inline const std::list<SignaturePair>& getSignaturePairs() { return mSignaturePairs; }

		protected:
			std::list<SignaturePair> mSignaturePairs;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_SIGNATURE_MAP_H