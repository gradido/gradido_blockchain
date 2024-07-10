#include "gradido_blockchain/interaction/validate/TransferAmountRole.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"

namespace gradido {
	namespace interaction {
		namespace validate {
			void TransferAmountRole::run(
				Type type,
				std::string_view communityId,
				blockchain::AbstractProvider* blockchainProvider,
				data::ConstConfirmedTransactionPtr senderPreviousConfirmedTransaction,
				data::ConstConfirmedTransactionPtr recipientPreviousConfirmedTransaction
			) {
				if ((type & Type::SINGLE) == Type::SINGLE) {
					if (!mTransferAmount.communityId.empty() && !isValidCommunityAlias(mTransferAmount.communityId)) {
						throw TransactionValidationInvalidInputException("invalid character, only ascii", "coinCommunityId", "string");
					}
					if (!communityId.empty() && mTransferAmount.communityId == communityId) {
						throw TransactionValidationInvalidInputException(
							"coin communityId shouldn't be set if it is the same as blockchain communityId",
							"communityId", "hex"
						);
					}
					if (mTransferAmount.amount <= GradidoUnit(0.0)) {
						throw TransactionValidationInvalidInputException("zero or negative amount", "amount", "GradidoUnit");
					}
					validateEd25519PublicKey(mTransferAmount.pubkey, "sender");
				}
			}
		}
	}
}