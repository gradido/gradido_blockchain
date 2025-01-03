#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_TIMEOUT_DEFERRED_TRANSFER_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_TIMEOUT_DEFERRED_TRANSFER_ROLE_H

#include "AbstractRole.h"

namespace gradido {
	namespace interaction {
		namespace validate {
			class GradidoTimeoutDeferredTransferRole : public AbstractRole
			{
			public:
				GradidoTimeoutDeferredTransferRole(std::shared_ptr<const data::GradidoTimeoutDeferredTransfer> timeoutDeferredTransfer);

				void run(
					Type type,
					std::shared_ptr<blockchain::Abstract> blockchain,
					std::shared_ptr<const data::ConfirmedTransaction> senderPreviousConfirmedTransaction,
					std::shared_ptr<const data::ConfirmedTransaction> recipientPreviousConfirmedTransaction
				);
			protected:
				std::shared_ptr<const data::GradidoTimeoutDeferredTransfer> mTimeoutDeferredTransfer;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_TIMEOUT_DEFERRED_TRANSFER_ROLE_H