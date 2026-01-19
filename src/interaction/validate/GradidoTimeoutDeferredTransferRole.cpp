#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/Filter.h"
#include "gradido_blockchain/interaction/calculateAccountBalance/Context.h"
#include "gradido_blockchain/interaction/validate/GradidoTimeoutDeferredTransferRole.h"
#include "gradido_blockchain/interaction/validate/GradidoTransferRole.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

#include "date/date.h"

#include <memory>

using std::shared_ptr;

namespace gradido {
	using data::Timestamp, data::ConfirmedTransaction, data::GradidoTimeoutDeferredTransfer;

	namespace interaction {
		namespace validate {

			GradidoTimeoutDeferredTransferRole::GradidoTimeoutDeferredTransferRole(
				shared_ptr<const GradidoTimeoutDeferredTransfer> timeoutDeferredTransfer
			) : mTimeoutDeferredTransfer(timeoutDeferredTransfer)
			{
				assert(timeoutDeferredTransfer);
				// prepare for signature check
				mMinSignatureCount = 0;
			}

			void GradidoTimeoutDeferredTransferRole::run(Type type, ContextData& c) 
			{
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
					assert(c.senderBlockchain);
					auto deferredTransferEntry = c.senderBlockchain->getTransactionForId(mTimeoutDeferredTransfer->getDeferredTransferTransactionNr());
					if (!deferredTransferEntry) {
						throw TransactionValidationInvalidInputException(
							"deferredTransferTransactionNr is invalid, couldn't find transaction",
							"deferred_transfer_transaction_nr",
							"uint64",
							"std::shared_ptr<const blockchain::TransactionEntry>",
							"nullptr"
						);
					}
					auto deferredTransferConfirmed = deferredTransferEntry->getConfirmedTransaction()->getConfirmedAt();
					auto body = deferredTransferEntry->getTransactionBody();
					assert(body->isDeferredTransfer());
					auto timeoutDuration = body->getDeferredTransfer()->getTimeoutDuration();
					if(mCreatedAt.getAsTimepoint() != deferredTransferConfirmed.getAsTimepoint() + timeoutDuration.getAsDuration()) {
						throw TransactionValidationInvalidInputException(
							"createdAt is invalid, must be timeoutDuration after confirmed deferred transfer",
							"createdAt",
							"Timestamp",
							DataTypeConverter::timePointToString(deferredTransferConfirmed.getAsTimepoint() + timeoutDuration.getAsDuration()).data(),
							DataTypeConverter::timePointToString(mCreatedAt).data()
						);
					}

				}
				if ((type & Type::ACCOUNT) == Type::ACCOUNT) {
					assert(c.senderBlockchain);
					auto deferredTransferEntry = c.senderBlockchain->getTransactionForId(mTimeoutDeferredTransfer->getDeferredTransferTransactionNr());
					auto body = deferredTransferEntry->getTransactionBody();
					assert(body->isDeferredTransfer());
					auto deferredTransfer = deferredTransferEntry->getTransactionBody()->getDeferredTransfer();
					calculateAccountBalance::Context calculateAccountBalance(c.senderBlockchain);
					Timestamp beforeCreateAt(mCreatedAt.getSeconds(), mCreatedAt.getNanos() - 1000);
					auto balance = calculateAccountBalance.fromEnd(
						deferredTransfer->getRecipientPublicKey(), 
						beforeCreateAt,
						deferredTransfer->getTransfer().getSender().getCoinCommunityIdIndex()
					);
					if(GradidoUnit::zero() == balance) {
						throw TransactionValidationInvalidInputException(
							"deferred transfer balance is already empty",
							"deferredTransfer.balance",
							"GradidoUnit",
							"> 0",
							balance.toString().data()
						);
					}
				}
			}

		}
	}
}