#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/interaction/validate/GradidoTimeoutDeferredTransferRole.h"
#include "gradido_blockchain/interaction/validate/GradidoTransferRole.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

#include "date/date.h"

namespace gradido {
	namespace interaction {
		namespace validate {

			GradidoTimeoutDeferredTransferRole::GradidoTimeoutDeferredTransferRole(
				std::shared_ptr<const data::GradidoTimeoutDeferredTransfer> timeoutDeferredTransfer
			) : mTimeoutDeferredTransfer(timeoutDeferredTransfer)
			{
				assert(timeoutDeferredTransfer);
				// prepare for signature check
				mMinSignatureCount = 0;
			}

			void GradidoTimeoutDeferredTransferRole::run(
				Type type,
				std::shared_ptr<blockchain::Abstract> blockchain,
				std::shared_ptr<const data::ConfirmedTransaction> senderPreviousConfirmedTransaction,
				std::shared_ptr<const data::ConfirmedTransaction> recipientPreviousConfirmedTransaction
			) {
				if ((type & Type::SINGLE) == Type::SINGLE) {
					if (mTimeoutDeferredTransfer->getDeferredTransferTransactionNr() <= 3) {
						throw TransactionValidationInvalidInputException(
							"deferredTransferTransactionNr is to short",
							"deferred_transfer_transaction_nr",
							"uint64",
							"> 3",
							std::to_string(mTimeoutDeferredTransfer->getDeferredTransferTransactionNr()).data()
						);
					}
				}
				if ((type & Type::PREVIOUS) == Type::PREVIOUS) {
					assert(blockchain);
					if (!blockchain->getTransactionForId(mTimeoutDeferredTransfer->getDeferredTransferTransactionNr())) {
						throw TransactionValidationInvalidInputException(
							"deferredTransferTransactionNr is invalid, couldn't find transaction",
							"deferred_transfer_transaction_nr",
							"uint64",
							"std::shared_ptr<const blockchain::TransactionEntry>",
							"nullptr"
						);
					}
				}
			}

		}
	}
}