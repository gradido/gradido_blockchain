#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_DEFERRED_TRANSFER_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_DEFERRED_TRANSFER_ROLE_H

#include "AbstractRole.h"

namespace gradido {
	namespace interaction {
		namespace validate {
			class GradidoDeferredTransferRole : public AbstractRole
			{
			public:
				GradidoDeferredTransferRole(std::shared_ptr<const data::GradidoDeferredTransfer> deferredTransfer);

				void run(
					Type type,
					std::shared_ptr<blockchain::Abstract> blockchain,
					std::shared_ptr<const data::ConfirmedTransaction> senderPreviousConfirmedTransaction,
					std::shared_ptr<const data::ConfirmedTransaction> recipientPreviousConfirmedTransaction
				);
			protected:
				std::shared_ptr<const data::GradidoDeferredTransfer> mDeferredTransfer;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_DEFERRED_TRANSFER_ROLE_H