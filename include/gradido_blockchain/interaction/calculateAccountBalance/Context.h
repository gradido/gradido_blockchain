#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_CONTEXT_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_CONTEXT_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/data/BalanceOwner.h"
#include "gradido_blockchain/memory/Block.h"

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
				Context(const blockchain::Abstract& blockchain)
					: mBlockchain(blockchain) {}

				Context(const Context&) = delete;
				Context(Context&&) = delete;

				// calculate (final) balances after a specific transaction
				std::vector<data::AccountBalance> run(
					data::ConstGradidoTransactionPtr gradidoTransaction,
					const std::map<blockchain::TransactionRelationType, std::shared_ptr<const blockchain::TransactionEntry>>& relatedTransactions,
					Timepoint confirmedAt,
					uint64_t id
				);

				// calculate balance for a specific account for a specific date
				GradidoUnit run(
					memory::ConstBlockPtr publicKey,
					Timepoint balanceDate,
					uint64_t maxTransactionNr = 0, // last transaction nr to include
					std::string_view coinCommunityId = std::string_view() // for calculate only a specific coin color
				);
					
			protected:	
				//! confirmedAt confirmation date for transaction from which the body is
				std::shared_ptr<AbstractRole> getRole(std::shared_ptr<const data::TransactionBody> body, Timepoint confirmedAt);
				AccountBalance calculateAccountBalance(
					std::shared_ptr<const blockchain::TransactionEntry> lastOwnerTransaction,
					memory::ConstBlockPtr ownerPublicKey,
					Timepoint confirmedAt
				);
				const blockchain::Abstract& mBlockchain;
					
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_CONTEXT_H