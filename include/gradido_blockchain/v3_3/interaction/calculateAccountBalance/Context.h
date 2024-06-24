#ifndef __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_CALCULATE_ACCOUNT_BALANCE_CONTEXT_H
#define __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_CALCULATE_ACCOUNT_BALANCE_CONTEXT_H

#include "gradido_blockchain/export.h"
#include "AbstractRole.h"
#include "gradido_blockchain/v3_3/data/Protocol.h"
#include "gradido_blockchain/v3_3/blockchain/Abstract.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace calculateAccountBalance {
				class GRADIDOBLOCKCHAIN_EXPORT Context 
				{
				public:
					Context(const blockchain::Abstract& blockchain)
						: mBlockchain(blockchain) {}

					// calculate (final) balance after a specific transaction
					DecayDecimal run(
						data::ConstGradidoTransactionPtr gradidoTransaction,
						Timepoint confirmedAt,
						uint64_t id
					);

					// calculate balance for a specific account for a specific date
					DecayDecimal run(
						memory::ConstBlockPtr publicKey,
						Timepoint balanceDate,
						uint64_t maxTransactionNr = 0, // last transaction nr to include
						std::string_view coinCommunityId = nullptr // for calculate only a specific coin color
					);
					
				protected:	
					std::shared_ptr<AbstractRole> getRole(const data::TransactionBody& body);
					std::pair<Timepoint, DecayDecimal> calculateBookBackTimeoutedDeferredTransfer(
						std::shared_ptr<blockchain::TransactionEntry> transactionEntry
					);
					std::pair<Timepoint, DecayDecimal> calculateRedeemedDeferredTransferChange(
						const blockchain::DeferredRedeemedTransferPair& deferredRedeemingTransferPair
					);
					const blockchain::Abstract& mBlockchain;
					
				};
			}
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_CALCULATE_ACCOUNT_BALANCE_CONTEXT_H