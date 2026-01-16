#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_INBOUND_GRADIDO_TRANSFER_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_INBOUND_GRADIDO_TRANSFER_ROLE_H

#include "OutboundGradidoTransferRole.h"

namespace gradido {
	namespace interaction {
		namespace validate {
			class InboundGradidoTransferRole : public OutboundGradidoTransferRole
			{
			public:
				using OutboundGradidoTransferRole::OutboundGradidoTransferRole;

			protected:
				void validateAccount(
					const data::ConfirmedTransaction& ownBlockchainPreviousConfirmedTransaction,
					const data::ConfirmedTransaction& otherBlockchainPreviousConfirmedTransaction,
					std::shared_ptr<blockchain::Abstract> ownBlockchain,
					std::shared_ptr<blockchain::Abstract> otherBlockchain
				) override;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_INBOUND_GRADIDO_TRANSFER_ROLE_H