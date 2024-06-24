#include "gradido_blockchain/v3_3/interaction/validate/CommunityRootRole.h"
#include "gradido_blockchain/v3_3/interaction/validate/Exceptions.h"

#include "date/date.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace validate {

				CommunityRootRole::CommunityRootRole(const data::CommunityRoot& communityRoot)
					: mCommunityRoot(communityRoot) 
				{
					// prepare for signature check
					mMinSignatureCount = 1;
					mRequiredSignPublicKeys.push_back(communityRoot.pubkey);
				}

				void CommunityRootRole::run(
					Type type,
					std::string_view communityId,
					std::shared_ptr<blockchain::AbstractProvider> blockchainProvider,
					data::ConstConfirmedTransactionPtr previousConfirmedTransaction,
					data::ConstConfirmedTransactionPtr recipientPreviousConfirmedTransaction
				) {
					if ((type & Type::SINGLE) == Type::SINGLE) {
						validateEd25519PublicKey(mCommunityRoot.pubkey, "pubkey");
						validateEd25519PublicKey(mCommunityRoot.gmwPubkey, "gmwPubkey");
						validateEd25519PublicKey(mCommunityRoot.aufPubkey, "aufPubkey");

						const auto& pubkey = *mCommunityRoot.pubkey;
						const auto& gmwPubkey = *mCommunityRoot.gmwPubkey;
						const auto& aufPubkey = *mCommunityRoot.aufPubkey;

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
}