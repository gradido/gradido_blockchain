#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_CONTEXT_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_CONTEXT_H

#include "gradido_blockchain/export.h"

namespace gradido {
	namespace blockchain {
		class Abstract;
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

				// calculate (final) balance after a specific transaction
				GradidoUnit run(
					data::ConstGradidoTransactionPtr gradidoTransaction,
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
				std::shared_ptr<AbstractRole> getRole(const data::TransactionBody& body);
				std::pair<Timepoint, GradidoUnit> calculateBookBackTimeoutedDeferredTransfer(
					std::shared_ptr<const blockchain::TransactionEntry> transactionEntry
				);
				std::pair<Timepoint, GradidoUnit> calculateRedeemedDeferredTransferChange(
					const std::pair<std::shared_ptr<const blockchain::TransactionEntry>, std::shared_ptr<const blockchain::TransactionEntry>>& deferredRedeemingTransferPair
				);
				const blockchain::Abstract& mBlockchain;
					
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_CONTEXT_H