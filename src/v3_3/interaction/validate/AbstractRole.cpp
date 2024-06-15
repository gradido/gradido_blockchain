#include "gradido_blockchain/v3_3/interaction/validate/AbstractRole.h"
#include "gradido_blockchain/v3_3/interaction/validate/Exceptions.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"

#include <regex>

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace validate {
			
				std::regex g_RegExCommunityAlias("^[a-z0-9-]{3,120}$");

				bool AbstractRole::isValidCommunityAlias(const std::string& communityAlias)
				{
					return std::regex_match(communityAlias, g_RegExCommunityAlias);
				}

				void AbstractRole::validateEd25519PublicKey(const memory::ConstBlockPtr ed25519PublicKey, const char* name)
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

				void AbstractRole::validateEd25519Signature(const memory::ConstBlockPtr ed25519Signature, const char* name)
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


			}
		}
	}
}