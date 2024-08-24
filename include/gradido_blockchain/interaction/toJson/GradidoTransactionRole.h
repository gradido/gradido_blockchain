#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_TO_JSON_GRADIDO_TRANSACTION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_TO_JSON_GRADIDO_TRANSACTION_ROLE_H

#include "gradido_blockchain/data/GradidoTransaction.h"
#include "BodyBytesType.h"
#include "AbstractRole.h"

namespace gradido {
	namespace interaction {
		namespace toJson {
			class GradidoTransactionRole : public AbstractRole
			{
			public:
				GradidoTransactionRole(const data::GradidoTransaction& transaction, BodyBytesType format = BodyBytesType::JSON)
					: mTransaction(transaction), mFormat(format) {}

				rapidjson::Value composeJson(rapidjson::Document& rootDocument) const;
			protected:
				const data::GradidoTransaction& mTransaction;
				BodyBytesType mFormat;
			};
		}
	}
}


#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_TO_JSON_GRADIDO_TRANSACTION_ROLE_H