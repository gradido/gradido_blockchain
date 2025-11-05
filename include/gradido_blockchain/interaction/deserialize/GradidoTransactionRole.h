#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_GRADIDO_TRANSACTION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_GRADIDO_TRANSACTION_ROLE_H

#include "Protopuf.h"

namespace gradido {
	namespace data {
		class GradidoTransaction;
	}
	namespace interaction {
		namespace deserialize {

			class GradidoTransactionRole
			{
			public:
				GradidoTransactionRole(const GradidoTransactionMessage& message);
				~GradidoTransactionRole();

				inline std::unique_ptr<const data::GradidoTransaction> getGradidoTransaction() { return std::move(mGradidoTransaction); }
			protected:
				std::unique_ptr<const data::GradidoTransaction> mGradidoTransaction;
			};

		}
	}
}


#endif // __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_SERIALIZE_GRADIDO_TRANSACTION_ROLE_H