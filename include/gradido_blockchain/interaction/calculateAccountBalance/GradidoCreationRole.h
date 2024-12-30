#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_GRADIDO_CREATION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_GRADIDO_CREATION_ROLE_H

#include "AbstractRole.h"

namespace gradido {
	namespace data {
		class TransferAmount;
	}
	namespace interaction {
		namespace calculateAccountBalance {
			class GradidoCreationRole : public AbstractRole
			{
			public:
				using AbstractRole::AbstractRole;

				//! how much this transaction will reduce the account balance
				GradidoUnit getAmountCost(memory::ConstBlockPtr accountPublicKey) const {
					return 0.0;
				};

				virtual memory::ConstBlockPtr getRecipient() const {
					return mBody->getCreation()->getRecipient().getPublicKey();
				};
				virtual memory::ConstBlockPtr getSender() const {
					return nullptr;
				}
			protected:
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_CALCULATE_ACCOUNT_BALANCE_GRADIDO_CREATION_ROLE_H