#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CREATE_TRANSACTION_BY_EVENT_CONTEXT_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CREATE_TRANSACTION_BY_EVENT_CONTEXT_H

#include "gradido_blockchain/export.h"

#include <memory>

namespace gradido {
	namespace blockchain {
		class Abstract;
	}
	namespace data {
		class TransactionTriggerEvent;
		class GradidoTransaction;
	}
	namespace interaction {
		namespace createTransactionByEvent {
			class GRADIDOBLOCKCHAIN_EXPORT Context 
			{
			public:
				Context(std::shared_ptr<const blockchain::Abstract> blockchain)
					: mBlockchain(blockchain) {}
				~Context() {}

				std::unique_ptr<data::GradidoTransaction> run(std::shared_ptr<const data::TransactionTriggerEvent> transactionTriggerEvent);
			protected:
				std::shared_ptr<const blockchain::Abstract> mBlockchain;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_CREATE_TRANSACTION_BY_EVENT_CONTEXT_H