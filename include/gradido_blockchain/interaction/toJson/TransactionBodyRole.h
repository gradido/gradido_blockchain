#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_TO_JSON_TRANSACTION_BODY_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_TO_JSON_TRANSACTION_BODY_ROLE_H

#include "AbstractRole.h"

namespace gradido {
	namespace data {
		class TransactionBody;
		class GradidoTransfer;
		class TransferAmount;
	}
	namespace interaction {
		namespace toJson {
			class TransactionBodyRole : public AbstractRole
			{
			public:
				TransactionBodyRole(const data::TransactionBody& body)
					: mBody(body) {}

				rapidjson::Value composeJson(rapidjson::Document& rootDocument) const;
			protected:
				rapidjson::Value gradidoTransfer(const data::GradidoTransfer& data, rapidjson::Value& d, rapidjson::Document& rootDocument) const;
				rapidjson::Value transferAmount(const data::TransferAmount& data, rapidjson::Value& d, rapidjson::Document& rootDocument) const;
				const data::TransactionBody& mBody;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_TO_JSON_TRANSACTION_BODY_ROLE_H