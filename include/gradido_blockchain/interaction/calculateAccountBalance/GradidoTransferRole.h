#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_GRADIDO_TRANSFER_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_GRADIDO_TRANSFER_ROLE_H

#include "AbstractRole.h"
#include "gradido_blockchain/data/Protocol.h"

namespace gradido {
	namespace interaction {
		namespace calculateAccountBalance {
			class GradidoTransferRole : public AbstractRole
			{
			public:
				GradidoTransferRole(const data::GradidoTransfer& transfer)
					: mTransfer(transfer) {}

				inline bool isFinalBalanceForAccount(memory::ConstBlockPtr accountPublicKey) const {
					return mTransfer.sender.pubkey->isTheSame(accountPublicKey);
				}
				inline GradidoUnit getAmount() const {
					return mTransfer.sender.amount;
				}
				inline memory::ConstBlockPtr getFinalBalanceAddress() const {
					return mTransfer.sender.pubkey;
				}
			protected:
				const data::GradidoTransfer& mTransfer;

			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_GRADIDO_TRANSFER_ROLE_H