#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_GRADIDO_REDEEM_DEFERRED_TRANSFER_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_GRADIDO_REDEEM_DEFERRED_TRANSFER_ROLE_H

#include "AbstractRole.h"

namespace gradido {
	namespace interaction {
		namespace calculateAccountBalance {
			class GradidoRedeemDeferredTransferRole : public AbstractRole
			{
			public:
				GradidoRedeemDeferredTransferRole(
					std::shared_ptr<const data::TransactionBody> body,
					Timepoint confirmedAt,
					const blockchain::Abstract& blockchain
				) : AbstractRole(body)
				{
					auto transactionEntry = blockchain.getTransactionForId(body->getRedeemDeferredTransfer()->getDeferredTransferTransactionNr());
					mDeferredTransferConfirmedAt = transactionEntry->getConfirmedTransaction()->getConfirmedAt();
					mRedeemTransferConfirmedAt = confirmedAt;
					mDeferredTransfer = transactionEntry->getTransactionBody()->getDeferredTransfer();
				}

				//! how much this transaction will add to the account balance
				virtual GradidoUnit getAmountAdded(memory::ConstBlockPtr accountPublicKey) const;

				virtual memory::ConstBlockPtr getRecipient() const {
					return mBody->getRedeemDeferredTransfer()->getTransfer().getRecipient();
				}

			protected:
				std::shared_ptr<const data::GradidoDeferredTransfer> mDeferredTransfer;
				Timepoint mDeferredTransferConfirmedAt;
				Timepoint mRedeemTransferConfirmedAt;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_GRADIDO_REDEEM_DEFERRED_TRANSFER_ROLE_H