#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_TO_JSON_CONFIRMED_TRANSACTION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_TO_JSON_CONFIRMED_TRANSACTION_ROLE_H

#include "GradidoTransactionRole.h"

namespace gradido {
	namespace data {
		class ConfirmedTransaction;
	}
	namespace interaction {
		namespace toJson {
				
			class ConfirmedTransactionRole : public AbstractRole
			{
			public:
				ConfirmedTransactionRole(const data::ConfirmedTransaction& transaction, BodyBytesType format = BodyBytesType::JSON)
					: mTransaction(transaction), mFormat(format) {}

				rapidjson::Value composeJson(rapidjson::Document& rootDocument) const;
			protected:
				const data::ConfirmedTransaction& mTransaction;
				BodyBytesType mFormat;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_TO_JSON_CONFIRMED_TRANSACTION_ROLE_H