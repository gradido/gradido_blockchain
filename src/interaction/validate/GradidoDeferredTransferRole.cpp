#include "gradido_blockchain/interaction/validate/GradidoDeferredTransferRole.h"
#include "gradido_blockchain/interaction/validate/GradidoTransferRole.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/const.h"

#include "date/date.h"

namespace gradido {
	namespace interaction {
		namespace validate {

			GradidoDeferredTransferRole::GradidoDeferredTransferRole(const data::GradidoDeferredTransfer& deferredTransfer)
				: mDeferredTransfer(deferredTransfer) 
			{
				// prepare for signature check
				mMinSignatureCount = 1;
				mRequiredSignPublicKeys.push_back(deferredTransfer.transfer.sender.pubkey);
			}

			void GradidoDeferredTransferRole::run(
				Type type,
				std::string_view communityId,
				blockchain::AbstractProvider* blockchainProvider,
				data::ConstConfirmedTransactionPtr senderPreviousConfirmedTransaction,
				data::ConstConfirmedTransactionPtr recipientPreviousConfirmedTransaction
			) {
				if ((type & Type::SINGLE) == Type::SINGLE) {
					if (mDeferredTransfer.timeout.getAsTimepoint() - mConfirmedAt.getAsTimepoint() > GRADIDO_DEFERRED_TRANSFER_MAX_TIMEOUT_INTERVAL) {
						throw TransactionValidationInvalidInputException("timeout is to far away from confirmed date", "timeout", "timestamp");
					}
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