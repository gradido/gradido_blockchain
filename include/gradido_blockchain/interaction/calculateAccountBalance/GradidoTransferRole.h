#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_GRADIDO_TRANSFER_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_GRADIDO_TRANSFER_ROLE_H

#include "AbstractRole.h"

namespace gradido {
	namespace interaction {
		namespace calculateAccountBalance {
			class GradidoTransferRole : public AbstractRole
			{
			public:
				GradidoTransferRole(const data::GradidoTransfer& transfer)
					: mTransfer(transfer) {}

				inline bool isFinalBalanceForAccount(memory::ConstBlockPtr accountPublicKey) const {
					return mTransfer.getSender().getPubkey()->isTheSame(accountPublicKey);
				}
				//! how much this transaction will add to the account balance
				GradidoUnit getAmountAdded(memory::ConstBlockPtr accountPublicKey) const {
					if (mTransfer.getRecipient()->isTheSame(accountPublicKey)) {
						return mTransfer.getSender().getAmount();
					}
					return 0.0;
				}
				//! how much this transaction will reduce the account balance
				GradidoUnit getAmountCost(memory::ConstBlockPtr accountPublicKey) const {
					if (mTransfer.getSender().getPubkey()->isTheSame(accountPublicKey)) {
						return mTransfer.getSender().getAmount();
					}
					return 0.0;
				}
				GradidoUnit getDecayedAmount(Timepoint startDate, Timepoint endDate) const {
					return mTransfer.getSender().getAmount().calculateDecay(startDate, endDate);
				}
				inline memory::ConstBlockPtr getFinalBalanceAddress() const {
					return mTransfer.getSender().getPubkey();
				}
			protected:
				const data::GradidoTransfer& mTransfer;

			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_GRADIDO_TRANSFER_ROLE_H