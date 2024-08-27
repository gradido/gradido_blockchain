#include "gradido_blockchain/const.h"
#include "gradido_blockchain/interaction/validate/GradidoDeferredTransferRole.h"
#include "gradido_blockchain/interaction/validate/GradidoTransferRole.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

#include "date/date.h"

namespace gradido {
	namespace interaction {
		namespace validate {

			GradidoDeferredTransferRole::GradidoDeferredTransferRole(std::shared_ptr<const data::GradidoDeferredTransfer> deferredTransfer)
				: mDeferredTransfer(deferredTransfer) 
			{
				// prepare for signature check
				mMinSignatureCount = 1;
				mRequiredSignPublicKeys.push_back(deferredTransfer->getTransfer().getSender().getPubkey());
			}

			void GradidoDeferredTransferRole::run(
				Type type,
				std::string_view communityId,
				blockchain::AbstractProvider* blockchainProvider,
				data::ConstConfirmedTransactionPtr senderPreviousConfirmedTransaction,
				data::ConstConfirmedTransactionPtr recipientPreviousConfirmedTransaction
			) {
				if ((type & Type::SINGLE) == Type::SINGLE) {
					if (mDeferredTransfer->getTimeout().getAsTimepoint() - mConfirmedAt.getAsTimepoint() > GRADIDO_DEFERRED_TRANSFER_MAX_TIMEOUT_INTERVAL) {
						std::string expected = "<= " 
						+ DataTypeConverter::timePointToString(mConfirmedAt.getAsTimepoint()) 
						+ " + " 
						+ DataTypeConverter::timespanToString(GRADIDO_DEFERRED_TRANSFER_MAX_TIMEOUT_INTERVAL);						
						throw TransactionValidationInvalidInputException(
							"timeout is to far away from confirmed date", 
							"timeout", 
							"TimestampSeconds",
							expected.data(),
							DataTypeConverter::timePointToString(mDeferredTransfer->getTimeout().getAsTimepoint()).data()
						);
					}
					if (senderPreviousConfirmedTransaction) {
						if (senderPreviousConfirmedTransaction->getConfirmedAt() >= mDeferredTransfer->getTimeout()) {
							std::string expected = "> " + DataTypeConverter::timePointToString(senderPreviousConfirmedTransaction->getConfirmedAt());
							throw TransactionValidationInvalidInputException(
								"timeout must be greater than the confirmedAt date from the previous transaction of sender user",
								"timeout",
								"TimestampSeconds",
								expected.data(),
								DataTypeConverter::timePointToString(mDeferredTransfer->getTimeout().getAsTimepoint()).data()
							);
						}
					}
				}
				// make copy from GradidoTransfer
				auto transfer = std::make_shared<data::GradidoTransfer>(mDeferredTransfer->getTransfer());
				GradidoTransferRole transferRole(transfer, "");
				transferRole.setConfirmedAt(mConfirmedAt);
				transferRole.setCreatedAt(mCreatedAt);
				transferRole.run(type, communityId, blockchainProvider, senderPreviousConfirmedTransaction, recipientPreviousConfirmedTransaction);
			}

		}
	}
}