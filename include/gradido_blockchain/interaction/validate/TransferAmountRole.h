#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_TRANSFER_AMOUNT_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_TRANSFER_AMOUNT_ROLE_H

#include "AbstractRole.h"
#include "gradido_blockchain/data/Protocol.h"

namespace gradido {
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
					data::ConstConfirmedTransactionPtr senderPreviousConfirmedTransaction,
					data::ConstConfirmedTransactionPtr recipientPreviousConfirmedTransaction
				);

			protected:
				const data::TransferAmount& mTransferAmount;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_TRANSFER_AMOUNT_ROLE_H