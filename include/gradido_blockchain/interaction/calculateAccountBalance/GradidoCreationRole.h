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
				inline GradidoUnit getAmount() const {
					return mCreation.getRecipient().getAmount();
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