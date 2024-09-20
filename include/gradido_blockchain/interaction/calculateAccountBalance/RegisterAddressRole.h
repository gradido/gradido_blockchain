#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_REGISTER_ADDRESS_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_REGISTER_ADDRESS_ROLE_H

#include "AbstractRole.h"

namespace gradido {
	namespace interaction {
		namespace calculateAccountBalance {
			class RegisterAddressRole : public AbstractRole
			{
			public:
				RegisterAddressRole(const data::RegisterAddress& registerAddress)
					: mRegisterAddress(registerAddress) {}

				inline bool isFinalBalanceForAccount(memory::ConstBlockPtr accountPublicKey) const {
					return true;
				}
				//! how much this transaction will add to the account balance
				GradidoUnit getAmountAdded(memory::ConstBlockPtr accountPublicKey) const {
					return 0.0;
				};
				//! how much this transaction will reduce the account balance
				GradidoUnit getAmountCost(memory::ConstBlockPtr accountPublicKey) const {
					return 0.0;
				};
				GradidoUnit getDecayedAmount(Timepoint startDate, Timepoint endDate) const {
					return 0.0;
				}
				inline memory::ConstBlockPtr getFinalBalanceAddress() const {
					return mRegisterAddress.getAccountPublicKey();
				}
			protected:
				const data::RegisterAddress& mRegisterAddress;

			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_REGISTER_ADDRESS_ROLE_H