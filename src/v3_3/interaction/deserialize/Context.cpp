#include "gradido_blockchain/v3_3/interaction/deserialize/Context.h"
#include "gradido_blockchain/v3_3/interaction/deserialize/TransactionBodyRole.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace deserialize {
				void Context::run()
				{
					if (Type::TRANSACTION_BODY == mType || Type::UNKNOWN == mType) {
						try {
							auto [body, bufferEnd2] = message_coder<TransactionBodyMessage>::decode(mData->span());
							mTransactionBody = std::move(TransactionBodyRole(body).getTransactionBody());
							mType = Type::TRANSACTION_BODY;
						}
						catch (std::exception& ex) {
							printf("exception: %s\n", ex.what());
						}
					}
				}
			}
		}
	}
}

