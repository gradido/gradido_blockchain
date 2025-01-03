#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/interaction/validate/GradidoRedeemDeferredTransferRole.h"
#include "gradido_blockchain/interaction/validate/GradidoTransferRole.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

#include "date/date.h"

namespace gradido {
	namespace interaction {
		namespace validate {

			GradidoRedeemDeferredTransferRole::GradidoRedeemDeferredTransferRole(
				std::shared_ptr<const data::GradidoRedeemDeferredTransfer> redeemDeferredTransfer
			) : mRedeemDeferredTransfer(redeemDeferredTransfer)
			{
				assert(redeemDeferredTransfer);
				// prepare for signature check
				mMinSignatureCount = 1;
				mRequiredSignPublicKeys.push_back(redeemDeferredTransfer->getTransfer().getSender().getPublicKey());
			}

			void GradidoRedeemDeferredTransferRole::run(
				Type type,
				std::shared_ptr<blockchain::Abstract> blockchain,
				std::shared_ptr<const data::ConfirmedTransaction> senderPreviousConfirmedTransaction,
				std::shared_ptr<const data::ConfirmedTransaction> recipientPreviousConfirmedTransaction
			) {
				if ((type & Type::SINGLE) == Type::SINGLE) {
					if (mRedeemDeferredTransfer->getDeferredTransferTransactionNr() <= 3) {
						throw TransactionValidationInvalidInputException(
							"deferredTransferTransactionNr is to short",
							"deferred_transfer_transaction_nr",
							"uint64",
							"> 3",
							std::to_string(mRedeemDeferredTransfer->getDeferredTransferTransactionNr()).data()
						);
					}
				}
				if ((type & Type::PREVIOUS) == Type::PREVIOUS) {
					assert(blockchain);
					if (!blockchain->getTransactionForId(mRedeemDeferredTransfer->getDeferredTransferTransactionNr())) {
						throw TransactionValidationInvalidInputException(
							"deferredTransferTransactionNr is invalid, couldn't find transaction",
							"deferred_transfer_transaction_nr",
							"uint64",
							"std::shared_ptr<const blockchain::TransactionEntry>",
							"nullptr"
						);
					}
				}
				// make copy from GradidoTransfer
				auto transfer = std::make_shared<data::GradidoTransfer>(mRedeemDeferredTransfer->getTransfer());
				GradidoTransferRole transferRole(transfer, "");
				transferRole.setConfirmedAt(mConfirmedAt);
				transferRole.setCreatedAt(mCreatedAt);
				transferRole.run(type, blockchain, senderPreviousConfirmedTransaction, recipientPreviousConfirmedTransaction);
			}

		}
	}
}