#ifndef __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_CALCULATE_ACCOUNT_BALANCE_GRADIDO_DEFERRED_TRANSFER_ROLE_H
#define __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_CALCULATE_ACCOUNT_BALANCE_GRADIDO_DEFERRED_TRANSFER_ROLE_H

#include "AbstractRole.h"
#include "gradido_blockchain/v3_3/data/Protocol.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace calculateAccountBalance {
				class GradidoDeferredTransferRole : public AbstractRole
				{
				public:
					GradidoDeferredTransferRole(const data::GradidoDeferredTransfer& deferredTransfer, Timepoint balanceDate)
						: mDeferredTransfer(deferredTransfer), mBalanceDate(balanceDate) {}

					inline bool isFinalBalanceForAccount(memory::ConstBlockPtr accountPublicKey) const {
						return mDeferredTransfer.transfer.sender.pubkey->isTheSame(accountPublicKey);
					}
					DecayDecimal getBalance() const;
				protected:
					const data::GradidoDeferredTransfer& mDeferredTransfer;
					Timepoint mBalanceDate;
				};
			}
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_CALCULATE_ACCOUNT_BALANCE_GRADIDO_DEFERRED_TRANSFER_ROLE_H