#ifndef __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_TO_JSON_CONFIRMED_TRANSACTION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_TO_JSON_CONFIRMED_TRANSACTION_ROLE_H

#include "gradido_blockchain/v3_3/data/Protocol.h"
#include "GradidoTransactionRole.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace toJson {
				
				class ConfirmedTransactionRole : public AbstractRole
				{
				public:
					ConfirmedTransactionRole(const data::ConfirmedTransaction& transaction, BodyBytesFormat format = BodyBytesFormat::JSON)
						: mTransaction(transaction), mFormat(format) {}

					const char* run(bool pretty) const;
				protected:
					const data::ConfirmedTransaction& mTransaction;
					BodyBytesFormat mFormat;
				};
			}
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_TO_JSON_CONFIRMED_TRANSACTION_ROLE_H