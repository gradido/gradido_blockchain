#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_CONTEXT_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_CONTEXT_H

#include "gradido_blockchain/blockchain/TransactionRelationType.h"
#include "gradido_blockchain/memory/Block.h"

#include <map>

namespace gradido {
	namespace blockchain {
		class Abstract;
		class TransactionEntry;
	}
	namespace data {
		class AccountBalance;
	}
	namespace interaction {
		namespace calculateAccountBalance {
			class AbstractRole;

			class GRADIDOBLOCKCHAIN_EXPORT Context 
			{
			public:
				Context(std::shared_ptr<blockchain::Abstract> blockchain)
					: mBlockchain(blockchain) {}

				Context(const Context&) = delete;
				Context(Context&&) = delete;

				// calculate balance address starting from transaction >= startTransactionNr
				// calculate it newly, without relaying on AccountBalance except the first as startting point
				GradidoUnit run(uint64_t startTransactionNr, memory::ConstBlockPtr publicKey, Timepoint endDate) const;
					
			protected:	
				//! confirmedAt confirmation date for transaction from which the body is
				std::shared_ptr<AbstractRole> getRole(std::shared_ptr<const data::TransactionBody> body, Timepoint confirmedAt) const;
				std::shared_ptr<blockchain::Abstract> mBlockchain;
					
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_CONTEXT_H