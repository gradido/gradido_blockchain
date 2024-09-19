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
				//! how much this transaction will add to the account balance
				virtual GradidoUnit getAmountAdded(memory::ConstBlockPtr accountPublicKey) const = 0;
				//! how much this transaction will reduce the account balance
				virtual GradidoUnit getAmountCost(memory::ConstBlockPtr accountPublicKey) const = 0;
				//! behaviour for deferred transfer will differ to reduce rounding errors
				virtual GradidoUnit getDecayedAmount(Timepoint startDate, Timepoint endDate) const = 0;
				virtual ~AbstractRole() {}
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_ABSTRACT_ROLE_H