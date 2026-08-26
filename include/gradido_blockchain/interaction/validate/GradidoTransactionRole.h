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

				void run(Type type, ContextData& c);
			protected:
				const data::GradidoTransaction& mGradidoTransaction;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_TRANSACTION_ROLE_H