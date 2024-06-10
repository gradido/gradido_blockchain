#ifndef __GRADIDO_BLOCKCHAIN_v3_3_INTERACTION_SERIALIZE_CONFIRMED_TRANSACTION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_v3_3_INTERACTION_SERIALIZE_CONFIRMED_TRANSACTION_ROLE_H

#include "gradido_blockchain/v3_3/interaction/serialize/GradidoTransactionRole.h"
#include "Protopuf.h"
#include "AbstractRole.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace serialize {

				class ConfirmedTransactionRole : public AbstractRole
				{
				public:
					ConfirmedTransactionRole(const data::ConfirmedTransaction& confirmedTransaction) 
						: mConfirmedTransaction(confirmedTransaction), mGradidoTransactionRole(*mConfirmedTransaction.gradidoTransaction) {}
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
}

#endif 