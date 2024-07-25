#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_ABSTRACT_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_ABSTRACT_ROLE_H

#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/GradidoUnit.h"

namespace gradido {
	namespace interaction {
		namespace calculateAccountBalance {
			class AbstractRole
			{
			public:
				virtual bool isFinalBalanceForAccount(memory::ConstBlockPtr accountPublicKey) const = 0;
				virtual memory::ConstBlockPtr getFinalBalanceAddress() const = 0;
				virtual GradidoUnit getAmount() const = 0;
				virtual ~AbstractRole() {}
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_ABSTRACT_ROLE_H