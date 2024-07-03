#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_TO_JSON_TRANSACTION_BODY_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_TO_JSON_TRANSACTION_BODY_ROLE_H

#include "gradido_blockchain/data/Protocol.h"
#include "AbstractRole.h"

namespace gradido {
	namespace interaction {
		namespace toJson {
			class TransactionBodyRole : public AbstractRole
			{
			public:
				TransactionBodyRole(const data::TransactionBody& body)
					: mBody(body) {}

				const char* run(bool pretty) const;
			protected:
				rapidjson::Value gradidoTransfer(const data::GradidoTransfer& data, rapidjson::Document& d) const;
				rapidjson::Value transferAmount(const data::TransferAmount& data, rapidjson::Document& d) const;
				const data::TransactionBody& mBody;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_TO_JSON_TRANSACTION_BODY_ROLE_H