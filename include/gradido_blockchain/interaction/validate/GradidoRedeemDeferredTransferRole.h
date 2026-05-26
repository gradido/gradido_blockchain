#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_REDEEM_DEFERRED_TRANSFER_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_REDEEM_DEFERRED_TRANSFER_ROLE_H

#include "AbstractRole.h"
#include "gradido_blockchain_core/types/cross_group.h"

namespace gradido {
	namespace data {
		class GradidoRedeemDeferredTransfer;
	}
	namespace interaction {
		namespace validate {
			class GradidoRedeemDeferredTransferRole : public AbstractRole
			{
			public:
				GradidoRedeemDeferredTransferRole(std::shared_ptr<const data::GradidoRedeemDeferredTransfer> redeemDeferredTransfer);

				inline void setCrossGroupType(grdt_cross_group crossGroupType) { mCrossGroupType = crossGroupType; }

				void run(Type type, ContextData& c);
			protected:
				std::shared_ptr<const data::GradidoRedeemDeferredTransfer> mRedeemDeferredTransfer;
				grdt_cross_group mCrossGroupType;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_REDEEM_DEFERRED_TRANSFER_ROLE_H