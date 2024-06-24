#include "gradido_blockchain/v3_3/interaction/validate/AbstractRole.h"
#include "gradido_blockchain/v3_3/interaction/validate/Exceptions.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"

#include <regex>
#include <set>

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace validate {
			
				std::regex g_RegExCommunityAlias("^[a-z0-9-]{3,120}$");

				bool AbstractRole::isValidCommunityAlias(std::string_view communityAlias) const
				{
					return std::regex_match(communityAlias.begin(), communityAlias.end(), g_RegExCommunityAlias);
				}

				void AbstractRole::validateEd25519PublicKey(memory::ConstBlockPtr ed25519PublicKey, const char* name) const
				{
					if (!ed25519PublicKey) {
						throw TransactionValidationInvalidInputException("missing", name, "public key");
					}
					if (ed25519PublicKey->size() != ED25519_PUBLIC_KEY_SIZE) {
						throw TransactionValidationInvalidInputException("invalid size", name, "public key");
					}
					
					if(ed25519PublicKey->isEmpty()) {
						throw TransactionValidationInvalidInputException("empty", name, "public key");
					}
				}

				void AbstractRole::validateEd25519Signature(memory::ConstBlockPtr ed25519Signature, const char* name) const
				{
					if (!ed25519Signature) {
						throw TransactionValidationInvalidInputException("missing", name, "signature");
					}
					if (ed25519Signature->size() != ED25519_SIGNATURE_SIZE) {
						throw TransactionValidationInvalidInputException("invalid size", name, "signature");
					}

					if (ed25519Signature->isEmpty()) {
						throw TransactionValidationInvalidInputException("empty", name, "signature");
					}
				}

				void AbstractRole::checkRequiredSignatures(const data::SignatureMap& signatureMap) const
				{
					assert(mMinSignatureCount);

					auto& signPairs = signatureMap.signaturePairs;

					// check for doublets
					std::set<memory::Block> memoryBlockSet;
					for (auto& signPair : signPairs) {
						if (!memoryBlockSet.insert(*signPair.pubkey).second) {
							throw TransactionValidationInvalidInputException("double public key", "public key");
						};
					}
					memoryBlockSet.clear();
					
					// not enough
					if (mMinSignatureCount > signPairs.size()) {
						throw TransactionValidationMissingSignException(signPairs.size(), mMinSignatureCount);
					}
					// enough
					if (!mRequiredSignPublicKeys.size() && !mForbiddenSignPublicKeys.size()) {
						return;
					}
					// not all required keys could be fulfilled
					if (mRequiredSignPublicKeys.size() > signPairs.size()) {
						throw TransactionValidationRequiredSignMissingException(mRequiredSignPublicKeys);
					}					
					// check if specific signatures can be found
					// for only one signature pair we can speed it up
					if (signPairs.size() == 1) {
						auto& signPair = signPairs[0];
						// check for forbidden keys
						isPublicKeyForbidden(signPair.pubkey);

						// check for required keys
						if (mRequiredSignPublicKeys.size() == 1 && !signPair.pubkey->isTheSame(mRequiredSignPublicKeys[0])) {
							throw TransactionValidationRequiredSignMissingException(mRequiredSignPublicKeys);
						}
						return;
					}

					// prepare, make a copy from the vector, because entries will be removed from it
					// TODO: if we have in future vectors with many signature pairs, optimize for cache-hits or with hash map
					std::vector<memory::ConstBlockPtr> requiredKeys = mRequiredSignPublicKeys;

					memory::ConstBlockPtr lastPublicKey;
					for (auto& signPair : signPairs)
					{
						// check for forbidden keys
						isPublicKeyForbidden(signPair.pubkey);

						// check for required keys
						for (auto it = requiredKeys.begin(); it != requiredKeys.end(); it++) {
							if (signPair.pubkey->isTheSame(*it)) {
								it = requiredKeys.erase(it);
								break;
							}
						}
					}

					if (!requiredKeys.size()) return;

					throw TransactionValidationRequiredSignMissingException(requiredKeys);
				}

				void AbstractRole::isPublicKeyForbidden(memory::ConstBlockPtr pubkey) const
				{
					// check for forbidden keys
					for (auto forbiddenSignPublicKey : mForbiddenSignPublicKeys) {
						if (pubkey->isTheSame(forbiddenSignPublicKey)) {
							throw TransactionValidationForbiddenSignException(pubkey);
						}
					}
				}
			}
		}
	}
}