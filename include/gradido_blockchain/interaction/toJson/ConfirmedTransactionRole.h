#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_TO_JSON_CONFIRMED_TRANSACTION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_TO_JSON_CONFIRMED_TRANSACTION_ROLE_H

#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "GradidoTransactionRole.h"

namespace gradido {
	namespace interaction {
		namespace toJson {
				
			class ConfirmedTransactionRole : public AbstractRole
			{
			public:
				ConfirmedTransactionRole(const data::ConfirmedTransaction& transaction, BodyBytesFormat format = BodyBytesFormat::JSON)
					: mTransaction(transaction), mFormat(format) {}

				rapidjson::Value composeJson(rapidjson::Document& rootDocument) const;
			protected:
				const data::ConfirmedTransaction& mTransaction;
				BodyBytesFormat mFormat;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_TO_JSON_CONFIRMED_TRANSACTION_ROLE_H