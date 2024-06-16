#ifndef __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_DESERIALIZE_CONFIRMED_TRANSACTION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_DESERIALIZE_CONFIRMED_TRANSACTION_ROLE_H

#include "gradido_blockchain/v3_3/interaction/serialize/GradidoTransactionRole.h"
#include "Protopuf.h"

namespace gradido {
	namespace v3_3 {
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
}

#endif 