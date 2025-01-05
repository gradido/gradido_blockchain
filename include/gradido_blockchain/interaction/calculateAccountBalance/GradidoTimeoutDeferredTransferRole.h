#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_GRADIDO_TIMEOUT_DEFERRED_TRANSFER_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_GRADIDO_TIMEOUT_DEFERRED_TRANSFER_ROLE_H

#include "AbstractRole.h"

namespace gradido {
	namespace interaction {
		namespace calculateAccountBalance {
			class GradidoTimeoutDeferredTransferRole : public AbstractRole
			{
			public:
				GradidoTimeoutDeferredTransferRole(
					std::shared_ptr<const data::TransactionBody> body,
					std::shared_ptr<blockchain::Abstract> blockchain
				): AbstractRole(body)
				{
					auto transactionEntry = blockchain->getTransactionForId(body->getTimeoutDeferredTransfer()->getDeferredTransferTransactionNr());
					mDeferredTransfer = transactionEntry->getTransactionBody()->getDeferredTransfer();
				}

				//! how much this transaction will add to the account balance
				GradidoUnit getAmountAdded(memory::ConstBlockPtr accountPublicKey) const 
				{
					if (getRecipient()->isTheSame(accountPublicKey)) {
						return calculateRestAmount();
					}
					return 0.0;
				};
				//! how much this transaction will reduce the account balance
				GradidoUnit getAmountCost(memory::ConstBlockPtr accountPublicKey) const 
				{
					if (getTransferAmount().getPublicKey()->isTheSame(accountPublicKey)) {
						return calculateRestAmount();
					}
					return 0.0;
				};

				// helper
				virtual const data::TransferAmount& getTransferAmount() const {
					return mDeferredTransfer->getTransfer().getSender();
				}
				virtual memory::ConstBlockPtr getRecipient() const {
					return mDeferredTransfer->getTransfer().getSender().getPublicKey();
				}

				GradidoUnit calculateRestAmount() const;

			protected:
				std::shared_ptr<const data::GradidoDeferredTransfer> mDeferredTransfer;

			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_GRADIDO_TIMEOUT_DEFERRED_TRANSFER_ROLE_H