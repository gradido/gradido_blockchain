#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_ADVANCED_BLOCKCHAIN_FILTER_DEFERRED_TRANSFER_TRANSACTION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_ADVANCED_BLOCKCHAIN_FILTER_DEFERRED_TRANSFER_TRANSACTION_ROLE_H

#include "AbstractTransactionRole.h"

namespace gradido {

	namespace interaction {
		namespace advancedBlockchainFilter {
			class DeferredTransferTransactionRole : public AbstractTransactionRole
			{
			public:
				using AbstractTransactionRole::AbstractTransactionRole;

				memory::ConstBlockPtr getSenderPublicKey() const { return mBody->getTransferAmount().getPublicKey(); };
				memory::ConstBlockPtr getRecipientPublicKey() const { return mBody->getDeferredTransfer()->getTransfer().getRecipient(); };

			protected:
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_ADVANCED_BLOCKCHAIN_FILTER_DEFERRED_TRANSFER_TRANSACTION_ROLE_H