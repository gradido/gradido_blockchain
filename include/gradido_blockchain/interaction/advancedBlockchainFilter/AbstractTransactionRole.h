#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_ADVANCED_BLOCKCHAIN_FILTER_ABSTRACT_TRANSACTION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_ADVANCED_BLOCKCHAIN_FILTER_ABSTRACT_TRANSACTION_ROLE_H

#include "gradido_blockchain/blockchain/TransactionRelationType.h"
#include "gradido_blockchain/data/TransactionBody.h"
#include "gradido_blockchain/memory/Block.h"
#include <memory>


namespace gradido {
	namespace blockchain {
		class Abstract;
		class TransactionEntry;
	}
	namespace interaction {
		namespace advancedBlockchainFilter {
			class AbstractTransactionRole 
			{
			public:
				AbstractTransactionRole(std::shared_ptr<const data::TransactionBody> body)
					: mBody(body) {}
				virtual ~AbstractTransactionRole() {}

				virtual memory::ConstBlockPtr getSenderPublicKey() const = 0;
				virtual memory::ConstBlockPtr getRecipientPublicKey() const = 0;

			protected:
				std::shared_ptr<const data::TransactionBody> mBody;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_ADVANCED_BLOCKCHAIN_FILTER_ABSTRACT_TRANSACTION_ROLE_H