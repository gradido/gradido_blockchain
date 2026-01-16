#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_LOCAL_GRADIDO_TRANSFER_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_LOCAL_GRADIDO_TRANSFER_ROLE_H

#include "AbstractRole.h"

namespace gradido {
	namespace interaction {
		namespace validate {
			class LocalGradidoTransferRole : public AbstractRole
			{
			public:
				LocalGradidoTransferRole(std::shared_ptr<const data::GradidoTransfer> gradidoTransfer);

				virtual void run(
					Type type,
					std::shared_ptr<blockchain::Abstract> blockchain,
					std::shared_ptr<const data::ConfirmedTransaction> ownBlockchainPreviousConfirmedTransaction,
					std::shared_ptr<const data::ConfirmedTransaction> otherBlockchainPreviousConfirmedTransaction
				) override;

			protected:
				void validatePrevious(
					const data::ConfirmedTransaction& ownBlockchainPreviousConfirmedTransaction,
					std::shared_ptr<blockchain::Abstract> blockchain
				);
				//! both blockchain pointer could be the same
				//! \param senderBlockchain blockchain of sender account
				//! \param recipientBlockchain blockchain of recipient account
				void validateAccount(
					const data::ConfirmedTransaction& ownBlockchainPreviousConfirmedTransaction,
					std::shared_ptr<blockchain::Abstract> blockchain
				);

				std::shared_ptr<const data::GradidoTransfer> mGradidoTransfer;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_LOCAL_GRADIDO_TRANSFER_ROLE_H