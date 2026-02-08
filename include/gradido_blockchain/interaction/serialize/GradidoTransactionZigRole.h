#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_GRADIDO_TRANSACTION_ZIG_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_GRADIDO_TRANSACTION_ZIG_ROLE_H

#include "AbstractRole.h"

namespace gradido {
	namespace data {
		class GradidoTransaction;
	}
	namespace interaction {
		namespace serialize {

			class GradidoTransactionZigRole : public AbstractRole
			{
			public:
				GradidoTransactionZigRole(const data::GradidoTransaction& gradidoTransaction);
				~GradidoTransactionZigRole() {};

				memory::ConstBlockPtr run() const override;
				size_t calculateSerializedSize() const override;

			protected:
				const data::GradidoTransaction& mGradidoTransaction;
			};

		}
	}
}


#endif // __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_GRADIDO_TRANSACTION_ZIG_ROLE_H