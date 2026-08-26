#include "gradido_blockchain/data/adapter/publicKey.h"
#include "gradido_blockchain/data/SignatureMap.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

#include <map>
#include <set>

using std::map, std::set;

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

		bool SignatureMap::isPairing(const SignatureMap& other) const
		{
			if (mSignaturePairs.size() != other.mSignaturePairs.size()) {
				return false;
			}
			// most cases
			if (mSignaturePairs.size() == 1) {
				return mSignaturePairs[0].getPublicKey()->isTheSame(other.mSignaturePairs[0].getPublicKey());
			}
			set<PublicKey> pubkeys;
			for (const auto& sigPair : mSignaturePairs) {
				pubkeys.insert(adapter::toPublicKey(sigPair.getPublicKey()));
			}
			// compare with other
			for (const auto& otherSigPair : other.mSignaturePairs) {
				auto it = pubkeys.find(adapter::toPublicKey(otherSigPair.getPublicKey()));
				if (it == pubkeys.end()) {
					return false;
				}
			}
			return true;
		}
	}
}