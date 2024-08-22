#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_CONFIRMED_TRANSACTION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_CONFIRMED_TRANSACTION_ROLE_H

#include "gradido_blockchain/interaction/serialize/GradidoTransactionRole.h"
#include "Protopuf.h"
#include "AbstractRole.h"

namespace gradido {
	namespace interaction {
		namespace serialize {

			class ConfirmedTransactionRole : public AbstractRole
			{
			public:
				ConfirmedTransactionRole(const data::ConfirmedTransaction& confirmedTransaction) 
					: mConfirmedTransaction(confirmedTransaction), mGradidoTransactionRole(*mConfirmedTransaction.getGradidoTransaction()) {}
				~ConfirmedTransactionRole() {};

				RUM_IMPLEMENTATION
				ConfirmedTransactionMessage getMessage() const;
				size_t calculateSerializedSize() const;

			protected:
				const data::ConfirmedTransaction& mConfirmedTransaction;
				GradidoTransactionRole mGradidoTransactionRole;
			};

		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_CONFIRMED_TRANSACTION_ROLE_H