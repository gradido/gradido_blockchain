#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_TO_JSON_GRADIDO_TRANSACTION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_TO_JSON_GRADIDO_TRANSACTION_ROLE_H

#include "gradido_blockchain/data/Protocol.h"
#include "AbstractRole.h"

namespace gradido {
	namespace interaction {
		namespace toJson {
			enum class BodyBytesFormat
			{
				BASE64 = 1,
				HEX = 2,
				JSON = 4
			};
			class GradidoTransactionRole : public AbstractRole
			{
			public:
				GradidoTransactionRole(const data::GradidoTransaction& transaction, BodyBytesFormat format = BodyBytesFormat::JSON)
					: mTransaction(transaction), mFormat(format) {}

				const char* run(bool pretty) const;
			protected:
				const data::GradidoTransaction& mTransaction;
				BodyBytesFormat mFormat;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_TO_JSON_GRADIDO_TRANSACTION_ROLE_H