#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_REGISTER_ADDRESS_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_REGISTER_ADDRESS_ROLE_H

#include "AbstractRole.h"
#include "gradido_blockchain/data/Protocol.h"

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
				inline DecayDecimal getAmount() const {
					return 0.0;
				}
				inline memory::ConstBlockPtr getFinalBalanceAddress() const {
					return mRegisterAddress.accountPubkey;
				}
			protected:
				const data::RegisterAddress& mRegisterAddress;

			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_REGISTER_ADDRESS_ROLE_H