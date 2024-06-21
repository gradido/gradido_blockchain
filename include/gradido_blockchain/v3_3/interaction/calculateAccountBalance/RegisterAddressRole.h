#ifndef __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_CALCULATE_ACCOUNT_BALANCE_REGISTER_ADDRESS_ROLE_H
#define __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_CALCULATE_ACCOUNT_BALANCE_REGISTER_ADDRESS_ROLE_H

#include "AbstractRole.h"
#include "gradido_blockchain/v3_3/data/Protocol.h"

namespace gradido {
	namespace v3_3 {
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
					inline DecayDecimal getBalance() const {
						return static_cast<long>(0);
					}
				protected:
					const data::RegisterAddress& mRegisterAddress;

				};
			}
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_CALCULATE_ACCOUNT_BALANCE_REGISTER_ADDRESS_ROLE_H