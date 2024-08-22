#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_CONFIRMED_TRANSACTION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_CONFIRMED_TRANSACTION_ROLE_H

#include "gradido_blockchain/interaction/serialize/GradidoTransactionRole.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "Protopuf.h"

namespace gradido {
	namespace interaction {
		namespace deserialize {

			class ConfirmedTransactionRole
			{
			public:
				ConfirmedTransactionRole(const ConfirmedTransactionMessage& message);
				~ConfirmedTransactionRole() {}

				std::shared_ptr<const data::ConfirmedTransaction> getConfirmedTransaction() { return mConfirmedTransaction; }

			protected:
				data::ConfirmedTransactionPtr mConfirmedTransaction;
			};

		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_CONFIRMED_TRANSACTION_ROLE_H 