#ifndef GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_GRADIDO_TRANSACTION_ROLE_H
#define GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_GRADIDO_TRANSACTION_ROLE_H

#include "AbstractRole.h"

namespace gradido {
	namespace data {
		class GradidoTransaction;
	}
	namespace interaction {
		namespace serialize {

			class GradidoTransactionRole : public AbstractRole
			{
			public:
				GradidoTransactionRole(const data::GradidoTransaction& gradidoTransaction);
				~GradidoTransactionRole() {};

				memory::ConstBlockPtr run() const override;
				size_t calculateSerializedSize() const override;

			protected:
				const data::GradidoTransaction& mGradidoTransaction;
			};

		}
	}
}


#endif // GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_GRADIDO_TRANSACTION_ROLE_H