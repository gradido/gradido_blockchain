#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_GRADIDO_DEFERRED_TRANSFER_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_GRADIDO_DEFERRED_TRANSFER_ROLE_H

#include "AbstractRole.h"

namespace gradido {
	namespace interaction {
		namespace calculateAccountBalance {
			class GradidoDeferredTransferRole : public AbstractRole
			{
			public:
				GradidoDeferredTransferRole(const data::TransactionBody& body)
					: mBody(body)
				{
					assert(body.isDeferredTransfer());
				}

				inline bool isFinalBalanceForAccount(memory::ConstBlockPtr accountPublicKey) const {
					return getSender().getPubkey()->isTheSame(accountPublicKey);
				}
				//! how much this transaction will add to the account balance
				GradidoUnit getAmountAdded(memory::ConstBlockPtr accountPublicKey) const {
					if (getRecipient()->isTheSame(accountPublicKey)) {
						return getSender().getAmount();
					}
					return 0.0;
				};
				//! how much this transaction will reduce the account balance
				GradidoUnit getAmountCost(memory::ConstBlockPtr accountPublicKey) const {
					if (getSender().getPubkey()->isTheSame(accountPublicKey)) {
						return getSender().getAmount().calculateCompoundInterest(getCreatedAt(), getTimeout());
					}
					return 0.0;
				};
				GradidoUnit getDecayedAmount(Timepoint startDate, Timepoint endDate) const {
					assert(endDate <= getTimeout());
					return getSender().getAmount();
				}
				inline memory::ConstBlockPtr getFinalBalanceAddress() const {
					return getSender().getPubkey();
				}
				inline const data::TransferAmount& getSender() const {
					return mBody.getDeferredTransfer()->getTransfer().getSender();
				}
				inline const memory::ConstBlockPtr getRecipient() const {
					return mBody.getDeferredTransfer()->getTransfer().getRecipient();
				}
				inline const Timepoint getTimeout() const {
					return mBody.getDeferredTransfer()->getTimeout();
				}
				inline const Timepoint getCreatedAt() const {
					return mBody.getCreatedAt();
				}
			protected:				
				
				const data::TransactionBody& mBody;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_GRADIDO_DEFERRED_TRANSFER_ROLE_H