#ifndef __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_GRADIDO_TRANSACTION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_GRADIDO_TRANSACTION_ROLE_H

#include "AbstractRole.h"
#include "gradido_blockchain/v3_3/data/Protocol.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace validate {
				class GradidoTransactionRole : public AbstractRole
				{
				public:
					GradidoTransactionRole(const data::GradidoTransaction& gradidoTransaction)
						: mGradidoTransaction(gradidoTransaction) {}

					void run(
						Type type,
						const std::string& communityId,
						std::shared_ptr<AbstractBlockchainProvider> blockchainProvider,
						data::ConfirmedTransactionPtr senderPreviousConfirmedTransaction,
						data::ConfirmedTransactionPtr recipientPreviousConfirmedTransaction
					);
				protected:
					const data::GradidoTransaction& mGradidoTransaction;
				};
			}
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_GRADIDO_TRANSACTION_ROLE_H