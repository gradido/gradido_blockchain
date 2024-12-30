#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_ADVANCED_BLOCKCHAIN_FILTER_CONTEXT_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_ADVANCED_BLOCKCHAIN_FILTER_CONTEXT_H

#include "gradido_blockchain/export.h"
#include "AbstractTransactionRole.h"

namespace gradido {
	namespace blockchain {
		class Abstract;
	}
	namespace data {
		class ConfirmedTransaction;
	}
	namespace interaction {
		namespace advancedBlockchainFilter {
			class GRADIDOBLOCKCHAIN_EXPORT Context
			{
			public:
				Context(std::shared_ptr<blockchain::Abstract> blockchain)
					: mBlockchain(blockchain) {}
				~Context() {}

				std::shared_ptr<const blockchain::TransactionEntry> findRelatedTransaction(
					std::shared_ptr<const data::ConfirmedTransaction> parent,
					blockchain::TransactionRelationType type
				);
				std::shared_ptr<const blockchain::TransactionEntry> findRelatedTransaction(
					std::shared_ptr<const data::TransactionBody> parentBody,
					blockchain::TransactionRelationType type,
					uint64_t maxTransactionNr
				);

				std::shared_ptr<AbstractTransactionRole> getRole(std::shared_ptr<const data::TransactionBody> transactionBody);
				memory::ConstBlockPtr getPublicKeyForRelation(blockchain::TransactionRelationType type, std::shared_ptr<AbstractTransactionRole> role);

			protected:
				std::shared_ptr<blockchain::Abstract> mBlockchain;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_ADVANCED_BLOCKCHAIN_FILTER_CONTEXT_H