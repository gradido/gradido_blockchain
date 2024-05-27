#ifndef __GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_SIGNATURE_MAP_H
#define __GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_SIGNATURE_MAP_H

#include "SignaturePair.h"

namespace model {
	namespace protopuf {
		
		using SignatureMapMessage = message<
			message_field<"sig_pair", 1, SignaturePairMessage, repeated>
		>;
		
		class SignatureMap {
		public:
			SignatureMap(const SignatureMapMessage& data);
			~SignatureMap();

			const std::list<SignaturePair>& getSignaturePairs() { return mSignaturePairs; }

		protected:
			std::list<SignaturePair> mSignaturePairs;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_SIGNATURE_MAP_H