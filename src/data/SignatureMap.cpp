#include "gradido_blockchain/data/SignatureMap.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

namespace gradido {
	namespace data {

		void SignatureMap::push(const SignaturePair& signaturePair)
		{
			// check if pubkey already exist
			for (auto& signaturePairIt : mSignaturePairs) {
				if (signaturePair.getPublicKey()->isTheSame(signaturePairIt.getPublicKey())) {
					throw GradidoAlreadyExist("public key already exist in signature map of gradido transaction");
				}
			}
			mSignaturePairs.push_back(signaturePair);
		}
	}
}