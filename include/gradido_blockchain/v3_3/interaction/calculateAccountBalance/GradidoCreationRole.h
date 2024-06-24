#ifndef __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_CALCULATE_ACCOUNT_BALANCE_GRADIDO_CREATION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_CALCULATE_ACCOUNT_BALANCE_GRADIDO_CREATION_ROLE_H

#include "AbstractRole.h"
#include "gradido_blockchain/v3_3/data/Protocol.h"

namespace gradido {
	namespace v3_3 {
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
					inline DecayDecimal getAmount() const {
						return mCreation.recipient.amount;
					}
					inline memory::ConstBlockPtr getFinalBalanceAddress() const {
						return mCreation.recipient.pubkey;
					}
				protected:
					const data::GradidoCreation& mCreation;
				};
			}
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_CALCULATE_ACCOUNT_BALANCE_GRADIDO_CREATION_ROLE_H