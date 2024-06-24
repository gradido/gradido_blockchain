#ifndef __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_CALCULATE_ACCOUNT_BALANCE_GRADIDO_TRANSFER_ROLE_H
#define __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_CALCULATE_ACCOUNT_BALANCE_GRADIDO_TRANSFER_ROLE_H

#include "AbstractRole.h"
#include "gradido_blockchain/v3_3/data/Protocol.h"

namespace gradido {
	namespace v3_3 {
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
					inline DecayDecimal getAmount() const {
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
}

#endif //__GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_CALCULATE_ACCOUNT_BALANCE_GRADIDO_TRANSFER_ROLE_H