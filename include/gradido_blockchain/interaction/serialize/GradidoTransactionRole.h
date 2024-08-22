#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_GRADIDO_TRANSACTION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_GRADIDO_TRANSACTION_ROLE_H

#include "Protopuf.h"
#include "AbstractRole.h"
#include "SignatureMapRole.h"

namespace gradido {
	namespace interaction {
		namespace serialize {

			class GradidoTransactionRole : public AbstractRole
			{
			public:
				GradidoTransactionRole(const data::GradidoTransaction& gradidoTransaction) 
					: mGradidoTransaction(gradidoTransaction), mSigantureMapRole(gradidoTransaction.getSignatureMap()) {}
				~GradidoTransactionRole() {};

				RUM_IMPLEMENTATION
				GradidoTransactionMessage getMessage() const;
				size_t calculateSerializedSize() const;

			protected:
				const data::GradidoTransaction& mGradidoTransaction;
				SignatureMapRole mSigantureMapRole;
			};

		}
	}
}


#endif // __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_GRADIDO_TRANSACTION_ROLE_H