#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_GRADIDO_DEFERRED_TRANSFER_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_GRADIDO_DEFERRED_TRANSFER_ROLE_H

#include "AbstractRole.h"

namespace gradido {
	namespace interaction {
		namespace calculateAccountBalance {
			class GradidoDeferredTransferRole : public AbstractRole
			{
			public:
				GradidoDeferredTransferRole(const data::GradidoDeferredTransfer& deferredTransfer)
					: mDeferredTransfer(deferredTransfer) {}

				inline bool isFinalBalanceForAccount(memory::ConstBlockPtr accountPublicKey) const {
					return mDeferredTransfer.getTransfer().getSender().getPubkey()->isTheSame(accountPublicKey);
				}
				inline GradidoUnit getAmount() const {
					return mDeferredTransfer.getTransfer().getSender().getAmount();
				}
				inline memory::ConstBlockPtr getFinalBalanceAddress() const {
					return mDeferredTransfer.getTransfer().getSender().getPubkey();
				}
			protected:
				const data::GradidoDeferredTransfer& mDeferredTransfer;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_GRADIDO_DEFERRED_TRANSFER_ROLE_H