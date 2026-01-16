#include "gradido_blockchain/data/CommunityRoot.h"
#include "gradido_blockchain/interaction/validate/CommunityRootRole.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"

#include "date/date.h"

#include <memory>

using std::shared_ptr;

namespace gradido {
	using data::ConfirmedTransaction, data::CommunityRoot;
	namespace interaction {
		namespace validate {

			CommunityRootRole::CommunityRootRole(shared_ptr<const CommunityRoot> communityRoot)
				: mCommunityRoot(communityRoot) 
			{
				assert(communityRoot);
				// prepare for signature check
				mMinSignatureCount = 1;
				mRequiredSignPublicKeys.push_back(mCommunityRoot->getPublicKey());
			}

			void CommunityRootRole::run(Type type, ContextData& c) 
			{
				if ((type & Type::SINGLE) == Type::SINGLE) {
					validateEd25519PublicKey(mCommunityRoot->getPublicKey(), "pubkey");
					validateEd25519PublicKey(mCommunityRoot->getGmwPubkey(), "gmwPubkey");
					validateEd25519PublicKey(mCommunityRoot->getAufPubkey(), "aufPubkey");

					const auto& pubkey = *mCommunityRoot->getPublicKey();
					const auto& gmwPubkey = *mCommunityRoot->getGmwPubkey();
					const auto& aufPubkey = *mCommunityRoot->getAufPubkey();

					if (gmwPubkey == aufPubkey) { throw TransactionValidationException("gmw and auf are the same"); }
					if (pubkey == gmwPubkey) { throw TransactionValidationException("gmw and pubkey are the same"); }
					if (aufPubkey == pubkey) { throw TransactionValidationException("aufPubkey and pubkey are the same"); }
				}
				if ((type & Type::PREVIOUS) == Type::PREVIOUS) {
					if (c.senderPreviousConfirmedTransaction) {
						throw TransactionValidationException("community root must be the first transaction in the blockchain!");
					}
				}
			}
		}
	}
}