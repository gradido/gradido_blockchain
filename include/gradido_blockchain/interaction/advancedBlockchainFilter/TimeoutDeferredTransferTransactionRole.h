#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_ADVANCED_BLOCKCHAIN_FILTER_TIMEOUT_DEFERRED_TRANSFER_TRANSACTION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_ADVANCED_BLOCKCHAIN_FILTER_TIMEOUT_DEFERRED_TRANSFER_TRANSACTION_ROLE_H

#include "AbstractTransactionRole.h"

namespace gradido {

	namespace interaction {
		namespace advancedBlockchainFilter {
			class TimeoutDeferredTransferTransactionRole : public AbstractTransactionRole
			{
			public:
				TimeoutDeferredTransferTransactionRole(
					std::shared_ptr<const data::TransactionBody> body,
					std::shared_ptr<blockchain::Abstract> blockchain
				);

				// order reverted because timeout put back rest gdd
				memory::ConstBlockPtr getSenderPublicKey() const { return mDeferredTransferBody->getDeferredTransfer()->getTransfer().getRecipient(); };
				memory::ConstBlockPtr getRecipientPublicKey() const { return mDeferredTransferBody->getTransferAmount().getPublicKey(); };

			protected:
				std::shared_ptr<const data::TransactionBody> mDeferredTransferBody;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_ADVANCED_BLOCKCHAIN_FILTER_TIMEOUT_DEFERRED_TRANSFER_TRANSACTION_ROLE_H