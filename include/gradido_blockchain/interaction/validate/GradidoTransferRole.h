#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_TRANSFER_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_TRANSFER_ROLE_H

#include "AbstractRole.h"
#include "gradido_blockchain/data/Protocol.h"

namespace gradido {
	namespace interaction {
		namespace validate {
			class GradidoTransferRole : public AbstractRole
			{
			public:
				GradidoTransferRole(std::shared_ptr<const data::GradidoTransfer> gradidoTransfer, std::string_view otherCommunity);

				void run(
					Type type,
					std::string_view communityId,
					blockchain::AbstractProvider* blockchainProvider,
					data::ConstConfirmedTransactionPtr senderPreviousConfirmedTransaction,
					data::ConstConfirmedTransactionPtr recipientPreviousConfirmedTransaction
				);
				//! call only if this transfer belong to a deferred transfer, changing validation rules a bit
				inline void setDeferredTransfer() { mDeferredTransfer = true; }

			protected:
				void validatePrevious(
					const data::ConfirmedTransaction& previousConfirmedTransaction,
					std::shared_ptr<blockchain::Abstract> blockchain
				);
				//! both blockchain pointer could be the same
				//! \param senderBlockchain blockchain of sender account
				//! \param recipientBlockchain blockchain of recipient account
				void validateAccount(
					const data::ConfirmedTransaction& senderPreviousConfirmedTransaction,
					const data::ConfirmedTransaction& recipientPreviousConfirmedTransaction,
					std::shared_ptr<blockchain::Abstract> senderBlockchain,
					std::shared_ptr<blockchain::Abstract> recipientBlockchain
				);

				std::shared_ptr<const data::GradidoTransfer> mGradidoTransfer;
				std::string_view mOtherCommunity;
				bool mDeferredTransfer;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_TRANSFER_ROLE_H