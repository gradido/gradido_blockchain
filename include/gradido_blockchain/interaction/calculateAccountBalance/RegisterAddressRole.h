#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_REGISTER_ADDRESS_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_REGISTER_ADDRESS_ROLE_H

#include "AbstractRole.h"

namespace gradido {
	namespace interaction {
		namespace calculateAccountBalance {
			class RegisterAddressRole : public AbstractRole
			{
			public:
				using AbstractRole::AbstractRole;

				//! how much this transaction will add to the account balance
				GradidoUnit getAmountAdded(memory::ConstBlockPtr accountPublicKey) const {
					return 0.0;
				};
				//! how much this transaction will reduce the account balance
				GradidoUnit getAmountCost(memory::ConstBlockPtr accountPublicKey) const {
					return 0.0;
				};

				virtual memory::ConstBlockPtr getSender() const {
					return nullptr;
				}
				virtual memory::ConstBlockPtr getRecipient() const {
					return nullptr;
				}
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_REGISTER_ADDRESS_ROLE_H