#include "gradido_blockchain/blockchain/AbstractProvider.h"
#include "gradido_blockchain/blockchain/Exceptions.h"
#include "gradido_blockchain/const.h"
#include "gradido_blockchain/interaction/validate/AbstractRole.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"

#include <regex>
#include <set>
#include <vector>
#include <memory>

using memory::Block, memory::ConstBlockPtr;

using std::set;
using std::vector;
using std::shared_ptr;
using std::string, std::string_view;
using std::regex, std::regex_match;

namespace gradido {
	using data::SignatureMap;
	using blockchain::CommunityNotFoundException;

	namespace interaction {
		namespace validate {
			
			regex g_RegExCommunityAlias(COMMUNITY_ID_REGEX_STRING);

			bool AbstractRole::isValidCommunityAlias(string_view communityAlias) const
			{
				return regex_match(communityAlias.begin(), communityAlias.end(), g_RegExCommunityAlias);
			}

			void AbstractRole::validateEd25519PublicKey(ConstBlockPtr ed25519PublicKey, const char* name) const
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

			void AbstractRole::validateEd25519Signature(ConstBlockPtr ed25519Signature, const char* name) const
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

			void AbstractRole::checkRequiredSignatures(
				const SignatureMap& signatureMap,
				shared_ptr<blockchain::Abstract> blockchain
			) const 
			{
				auto& signPairs = signatureMap.getSignaturePairs();

				// check for doublets
				if (signPairs.size() > 1) {
					set<Block> memoryBlockSet;
					for (auto& signPair : signPairs) {
						if (!memoryBlockSet.insert(*signPair.getPublicKey()).second) {
							throw TransactionValidationInvalidInputException("double public key", "public key");
						};
					}
					memoryBlockSet.clear();
				}
					
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
					isPublicKeyForbidden(signPair.getPublicKey());

					// check for required keys
					if (mRequiredSignPublicKeys.size() == 1 && !signPair.getPublicKey()->isTheSame(mRequiredSignPublicKeys[0])) {
						throw TransactionValidationRequiredSignMissingException(mRequiredSignPublicKeys);
					}
					return;
				}

				// prepare, make a copy from the vector, because entries will be removed from it
				// TODO: if we have in future vectors with many signature pairs, optimize for cache-hits or with hash map
				vector<ConstBlockPtr> requiredKeys = mRequiredSignPublicKeys;

				ConstBlockPtr lastPublicKey;
				for (auto& signPair : signPairs)
				{
					// check for forbidden keys
					isPublicKeyForbidden(signPair.getPublicKey());

					// check for required keys
					for (auto it = requiredKeys.begin(); it != requiredKeys.end(); it++) {
						if (signPair.getPublicKey()->isTheSame(*it)) {
							it = requiredKeys.erase(it);
							break;
						}
					}
				}

				if (!requiredKeys.size()) return;

				throw TransactionValidationRequiredSignMissingException(requiredKeys);
			}

			void AbstractRole::isPublicKeyForbidden(ConstBlockPtr pubkey) const
			{
				// check for forbidden keys
				for (const auto& forbiddenSignPublicKey : mForbiddenSignPublicKeys) {
					if (pubkey->isTheSame(forbiddenSignPublicKey)) {
						throw TransactionValidationForbiddenSignException(pubkey);
					}
				}
			}
			shared_ptr<blockchain::Abstract> AbstractRole::findBlockchain(
				blockchain::AbstractProvider* blockchainProvider,
				string_view communityId,
				const char* callerFunction
			) {
				if (!blockchainProvider) {
					throw GradidoNullPointerException(
						"missing blockchain provider for interaction::validate Type::MONTH_RANGE",
						"blockchain::AbstractProvider*",
						callerFunction
					);
				}
				auto blockchain = blockchainProvider->findBlockchain(communityId);
				if (!blockchain) {
					throw CommunityNotFoundException("missing blockchain for interaction::validate", communityId);
				}
				return blockchain;
			}
		}
	}
}