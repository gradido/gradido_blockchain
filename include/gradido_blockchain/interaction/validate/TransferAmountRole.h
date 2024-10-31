#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_TRANSFER_AMOUNT_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_TRANSFER_AMOUNT_ROLE_H

#include "AbstractRole.h"

namespace gradido {
	namespace data {
		class TransferAmount;
	}
	namespace interaction {
		namespace validate {
			class TransferAmountRole : public AbstractRole
			{
			public:
				TransferAmountRole(const data::TransferAmount& transferAmount) : mTransferAmount(transferAmount) {}

				void run(
					Type type,
					std::string_view communityId,
					blockchain::AbstractProvider* blockchainProvider,
					std::shared_ptr<const data::ConfirmedTransaction> senderPreviousConfirmedTransaction,
					std::shared_ptr<const data::ConfirmedTransaction> recipientPreviousConfirmedTransaction
				);

			protected:
				const data::TransferAmount& mTransferAmount;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_TRANSFER_AMOUNT_ROLE_H