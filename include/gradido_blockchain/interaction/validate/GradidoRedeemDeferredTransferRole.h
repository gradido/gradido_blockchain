#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_REDEEM_DEFERRED_TRANSFER_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_REDEEM_DEFERRED_TRANSFER_ROLE_H

#include "AbstractRole.h"

namespace gradido {
	namespace interaction {
		namespace validate {
			class GradidoRedeemDeferredTransferRole : public AbstractRole
			{
			public:
				GradidoRedeemDeferredTransferRole(std::shared_ptr<const data::GradidoRedeemDeferredTransfer> redeemDeferredTransfer);

				void run(
					Type type,
					std::shared_ptr<blockchain::Abstract> blockchain,
					std::shared_ptr<const data::ConfirmedTransaction> ownBlockchainPreviousConfirmedTransaction,
					std::shared_ptr<const data::ConfirmedTransaction> otherBlockchainPreviousConfirmedTransaction
				);
			protected:
				std::shared_ptr<const data::GradidoRedeemDeferredTransfer> mRedeemDeferredTransfer;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_REDEEM_DEFERRED_TRANSFER_ROLE_H