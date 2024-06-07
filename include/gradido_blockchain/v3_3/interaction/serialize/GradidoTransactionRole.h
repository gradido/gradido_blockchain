#ifndef __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_SERIALIZE_GRADIDO_TRANSACTION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_SERIALIZE_GRADIDO_TRANSACTION_ROLE_H

#include "gradido_blockchain/v3_3/data/Protocol.h"
#include "gradido_blockchain/v3_3/data/Protopuf.h"
#include "AbstractRole.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace serialize {

				class GradidoTransactionRole : public AbstractRole
				{
				public:
					GradidoTransactionRole(const data::GradidoTransaction& gradidoTransaction) : mGradidoTransaction(gradidoTransaction) {}
					~GradidoTransactionRole() {};

					RUM_IMPLEMENTATION
					data::GradidoTransactionMessage getMessage() const;
					size_t calculateSerializedSize() const;

				protected:
					const data::GradidoTransaction& mGradidoTransaction;
				};

			}
		}
	}
}


#endif // __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_SERIALIZE_GRADIDO_TRANSACTION_ROLE_H