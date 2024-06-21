#ifndef __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_GRADIDO_TRANSFER_ROLE_H
#define __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_GRADIDO_TRANSFER_ROLE_H

#include "AbstractRole.h"
#include "gradido_blockchain/v3_3/data/Protocol.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace validate {
				class GradidoTransferRole : public AbstractRole
				{
				public:
					GradidoTransferRole(const data::GradidoTransfer& gradidoTransfer, const std::string& otherCommunity);

					void run(
						Type type,
						const std::string& communityId,
						std::shared_ptr<blockchain::AbstractProvider> blockchainProvider,
						data::ConfirmedTransactionPtr senderPreviousConfirmedTransaction,
						data::ConfirmedTransactionPtr recipientPreviousConfirmedTransaction
					);
					//! call only if this transfer belong to a deferred transfer, changing validation rules a bit
					inline void setDeferredTransfer() { mDeferredTransfer = true; }

				protected:
					void validateSingle(const std::string& communityId);
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

					const data::GradidoTransfer& mGradidoTransfer;
					std::string_view mOtherCommunity;
					bool mDeferredTransfer;
				};
			}
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_GRADIDO_TRANSFER_ROLE_H