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
			inline BodyBytesFormat operator | (BodyBytesFormat lhs, BodyBytesFormat rhs)
			{
				using T = std::underlying_type_t <BodyBytesFormat>;
				return static_cast<BodyBytesFormat>(static_cast<T>(lhs) | static_cast<T>(rhs));
			}

			inline BodyBytesFormat operator & (BodyBytesFormat lhs, BodyBytesFormat rhs)
			{
				using T = std::underlying_type_t <BodyBytesFormat>;
				return static_cast<BodyBytesFormat>(static_cast<T>(lhs) & static_cast<T>(rhs));
			}
			class GradidoTransactionRole : public AbstractRole
			{
			public:
				GradidoTransactionRole(const data::GradidoTransaction& transaction, BodyBytesFormat format = BodyBytesFormat::JSON)
					: mTransaction(transaction), mFormat(format) {}

				rapidjson::Value composeJson(rapidjson::Document& rootDocument) const;
			protected:
				const data::GradidoTransaction& mTransaction;
				BodyBytesFormat mFormat;
			};
		}
	}
}


#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_TO_JSON_GRADIDO_TRANSACTION_ROLE_H