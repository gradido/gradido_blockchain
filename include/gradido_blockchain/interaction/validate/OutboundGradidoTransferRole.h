#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_OUTBOUND_GRADIDO_TRANSFER_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_OUTBOUND_GRADIDO_TRANSFER_ROLE_H

#include "LocalGradidoTransferRole.h"

namespace gradido {
	namespace interaction {
		namespace validate {
			class OutboundGradidoTransferRole : public LocalGradidoTransferRole
			{
			public:
				OutboundGradidoTransferRole(std::shared_ptr<const data::GradidoTransfer> gradidoTransfer, std::string_view otherCommunity);

				void run(
					Type type,
					std::shared_ptr<blockchain::Abstract> blockchain,
					std::shared_ptr<const data::ConfirmedTransaction> ownBlockchainPreviousConfirmedTransaction,
					std::shared_ptr<const data::ConfirmedTransaction> otherBlockchainPreviousConfirmedTransaction
				) override;

			protected:
				virtual void validateAccount(
					const data::ConfirmedTransaction& ownBlockchainPreviousConfirmedTransaction,
					const data::ConfirmedTransaction& otherBlockchainPreviousConfirmedTransaction,
					std::shared_ptr<blockchain::Abstract> ownBlockchain,
					std::shared_ptr<blockchain::Abstract> otherBlockchain
				);

				std::string_view mOtherCommunity;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_OUTBOUND_GRADIDO_TRANSFER_ROLE_H