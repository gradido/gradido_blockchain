#ifndef __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_CALCULATE_ACCOUNT_BALANCE_ABSTRACT_ROLE_H
#define __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_CALCULATE_ACCOUNT_BALANCE_ABSTRACT_ROLE_H

#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/lib/DecayDecimal.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace calculateAccountBalance {
				class AbstractRole
				{
				public:
					virtual bool isFinalBalanceForAccount(memory::ConstBlockPtr accountPublicKey) const = 0;
					virtual DecayDecimal getAmount() const = 0;
					virtual ~AbstractRole() {}
				};
			}
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_CALCULATE_ACCOUNT_BALANCE_ABSTRACT_ROLE_H