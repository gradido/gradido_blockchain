#include "gradido_blockchain/v3_3/interaction/validate/GradidoDeferredTransferRole.h"
#include "gradido_blockchain/v3_3/interaction/validate/GradidoTransferRole.h"
#include "gradido_blockchain/v3_3/interaction/validate/Exceptions.h"

#include "date/date.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace validate {

				void GradidoDeferredTransferRole::run(
					Type type,
					const std::string& communityId,
					std::shared_ptr<AbstractBlockchainProvider> blockchainProvider,
					data::ConfirmedTransactionPtr senderPreviousConfirmedTransaction,
					data::ConfirmedTransactionPtr recipientPreviousConfirmedTransaction
				) {
					if ((type & Type::SINGLE) == Type::SINGLE) {
						if (senderPreviousConfirmedTransaction) {
							if (senderPreviousConfirmedTransaction->confirmedAt >= mDeferredTransfer.timeout) {
								throw TransactionValidationInvalidInputException("timeout is already in the past", "timeout", "timestamp");
							}
						}
					}
					GradidoTransferRole transferRole(mDeferredTransfer.transfer, "");
					transferRole.setConfirmedAt(mConfirmedAt);
					transferRole.run(type, communityId, blockchainProvider, senderPreviousConfirmedTransaction, recipientPreviousConfirmedTransaction);
				}

			}
		}
	}
}