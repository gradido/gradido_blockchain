#include "gradido_blockchain/data/CommunityRoot.h"
#include "gradido_blockchain/interaction/validate/CommunityRootRole.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"

#include "date/date.h"

namespace gradido {
	namespace interaction {
		namespace validate {

			CommunityRootRole::CommunityRootRole(std::shared_ptr<const data::CommunityRoot> communityRoot)
				: mCommunityRoot(communityRoot) 
			{
				assert(communityRoot);
				// prepare for signature check
				mMinSignatureCount = 1;
				mRequiredSignPublicKeys.push_back(mCommunityRoot->getPublicKey());
			}

			void CommunityRootRole::run(
				Type type,
				std::shared_ptr<blockchain::Abstract> blockchain,
				std::shared_ptr<const data::ConfirmedTransaction> previousConfirmedTransaction,
				std::shared_ptr<const data::ConfirmedTransaction> recipientPreviousConfirmedTransaction
			) {
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
					if (previousConfirmedTransaction) {
						throw TransactionValidationException("community root must be the first transaction in the blockchain!");
					}
				}
			}
		}
	}
}