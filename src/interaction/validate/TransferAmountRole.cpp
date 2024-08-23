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
					auto& coinCommunityId = mTransferAmount.getCommunityId();
					if (!coinCommunityId.empty() && !isValidCommunityAlias(coinCommunityId)) {
						throw TransactionValidationInvalidInputException("invalid character, only ascii", "coinCommunityId", "string");
					}
					if (!communityId.empty() && coinCommunityId == communityId) {
						throw TransactionValidationInvalidInputException(
							"coin communityId shouldn't be set if it is the same as blockchain communityId",
							"communityId", "hex"
						);
					}
					if (mTransferAmount.getAmount() <= GradidoUnit(0.0)) {
						throw TransactionValidationInvalidInputException("zero or negative amount", "amount", "GradidoUnit");
					}
					validateEd25519PublicKey(mTransferAmount.getPubkey(), "sender");
				}
			}
		}
	}
}