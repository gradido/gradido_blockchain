#include "gradido_blockchain/data/SignatureMap.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

#include <map>

using std::map;

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
		bool SignatureMap::isTheSame(const SignatureMap& other) const
		{
			if (mSignaturePairs.size() != other.mSignaturePairs.size()) {
				return false;
			}
			// most cases
			if (mSignaturePairs.size() == 1) {
				return mSignaturePairs[0] == other.mSignaturePairs[0];
			}
			
			// we use map because the order isn't neccessary the same
			map<int64_t, const SignaturePair&> ownHashes;
			for (const auto& sigPair : mSignaturePairs) {
				ownHashes.insert({ sigPair.hash(), sigPair });
			}
			// compare with other hashes
			for (const auto& otherSigPair : other.mSignaturePairs) {
				auto it = ownHashes.find(otherSigPair.hash());
				if (it == ownHashes.end()) {
					return false;
				}
				if (it->second != otherSigPair) {
					return false;
				}
			}
			return true;
		}
	}
}