#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_TRANSACTION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_TRANSACTION_ROLE_H

#include "AbstractRole.h"

namespace gradido {
	namespace data {
		class GradidoTransaction;
	}

	namespace interaction {
		namespace validate {
			class GradidoTransactionRole : public AbstractRole
			{
			public:
				GradidoTransactionRole(const data::GradidoTransaction& gradidoTransaction)
					: mGradidoTransaction(gradidoTransaction) {}

				void run(
					Type type,
					std::shared_ptr<blockchain::Abstract> blockchain,
					std::shared_ptr<const data::ConfirmedTransaction> ownBlockchainPreviousConfirmedTransaction,
					std::shared_ptr<const data::ConfirmedTransaction> otherBlockchainPreviousConfirmedTransaction
				);
			protected:
				const data::GradidoTransaction& mGradidoTransaction;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_TRANSACTION_ROLE_H