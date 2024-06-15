#include "gradido_blockchain/v3_3/interaction/validate/CommunityRootRole.h"
#include "gradido_blockchain/v3_3/interaction/validate/Exceptions.h"

#include "date/date.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace validate {

				void CommunityRootRole::run(
					Type type,
					const std::string& communityId,
					std::shared_ptr<AbstractBlockchainProvider> blockchainProvider,
					data::ConfirmedTransactionPtr previousConfirmedTransaction,
					data::ConfirmedTransactionPtr recipientPreviousConfirmedTransaction
				) {
					if ((type & Type::SINGLE) == Type::SINGLE) {
						validate25519PublicKey(mCommunityRoot.pubkey, "pubkey");
						validate25519PublicKey(mCommunityRoot.gmwPubkey, "gmwPubkey");
						validate25519PublicKey(mCommunityRoot.aufPubkey, "aufPubkey");

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