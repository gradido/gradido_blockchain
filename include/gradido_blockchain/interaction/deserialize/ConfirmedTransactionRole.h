#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_CONFIRMED_TRANSACTION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_CONFIRMED_TRANSACTION_ROLE_H

#include "Protopuf.h"

namespace gradido {
	namespace data {
		class ConfirmedTransaction;
	}
	namespace interaction {
		namespace deserialize {

			class ConfirmedTransactionRole
			{
			public:
				ConfirmedTransactionRole(const ConfirmedTransactionMessage& message, uint32_t communityIdIndex);
				~ConfirmedTransactionRole() {}

				std::shared_ptr<const data::ConfirmedTransaction> getConfirmedTransaction() { return mConfirmedTransaction; }

			protected:
				std::shared_ptr<data::ConfirmedTransaction> mConfirmedTransaction;
			};

		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_CONFIRMED_TRANSACTION_ROLE_H 