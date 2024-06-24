#include "gradido_blockchain/v3_3/interaction/validate/GradidoDeferredTransferRole.h"
#include "gradido_blockchain/v3_3/interaction/validate/GradidoTransferRole.h"
#include "gradido_blockchain/v3_3/interaction/validate/Exceptions.h"

#include "date/date.h"

namespace gradido {
	namespace v3_3 {
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
					std::shared_ptr<blockchain::AbstractProvider> blockchainProvider,
					data::ConstConfirmedTransactionPtr senderPreviousConfirmedTransaction,
					data::ConstConfirmedTransactionPtr recipientPreviousConfirmedTransaction
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