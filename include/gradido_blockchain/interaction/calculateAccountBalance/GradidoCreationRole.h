#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_GRADIDO_CREATION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_GRADIDO_CREATION_ROLE_H

#include "AbstractRole.h"

namespace gradido {
	namespace interaction {
		namespace calculateAccountBalance {
			class GradidoCreationRole : public AbstractRole
			{
			public:
				GradidoCreationRole(const data::GradidoCreation& creation)
					: mCreation(creation) {}

				inline bool isFinalBalanceForAccount(memory::ConstBlockPtr accountPublicKey) const {
					return true;
				}
				//! how much this transaction will add to the account balance
				GradidoUnit getAmountAdded(memory::ConstBlockPtr accountPublicKey) const {
					if (mCreation.getRecipient().getPubkey()->isTheSame(accountPublicKey)) {
						return mCreation.getRecipient().getAmount();
					}
					return 0.0;
				};
				//! how much this transaction will reduce the account balance
				GradidoUnit getAmountCost(memory::ConstBlockPtr accountPublicKey) const {
					return 0.0;
				};
				GradidoUnit getDecayedAmount(Timepoint startDate, Timepoint endDate) const {
					return mCreation.getRecipient().getAmount().calculateDecay(startDate, endDate);
				}
				inline memory::ConstBlockPtr getFinalBalanceAddress() const {
					return mCreation.getRecipient().getPubkey();
				}
			protected:
				const data::GradidoCreation& mCreation;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_CALCULATE_ACCOUNT_BALANCE_GRADIDO_CREATION_ROLE_H