#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_TRANSACTION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_TRANSACTION_ROLE_H

#include "AbstractRole.h"

namespace gradido {
	namespace interaction {
		namespace validate {
			class GradidoTransactionRole : public AbstractRole
			{
			public:
				GradidoTransactionRole(const data::GradidoTransaction& gradidoTransaction)
					: mGradidoTransaction(gradidoTransaction) {}

				void run(
					Type type,
					std::string_view communityId,
					blockchain::AbstractProvider* blockchainProvider,
					std::shared_ptr<const data::ConfirmedTransaction> senderPreviousConfirmedTransaction,
					std::shared_ptr<const data::ConfirmedTransaction> recipientPreviousConfirmedTransaction
				);
			protected:
				const data::GradidoTransaction& mGradidoTransaction;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_TRANSACTION_ROLE_H