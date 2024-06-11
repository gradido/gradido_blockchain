#include "gradido_blockchain/v3_3/interaction/deserialize/Context.h"
#include "gradido_blockchain/v3_3/interaction/deserialize/TransactionBodyRole.h"
#include "gradido_blockchain/v3_3/interaction/deserialize/GradidoTransactionRole.h"
#include "gradido_blockchain/v3_3/interaction/deserialize/ConfirmedTransactionRole.h"
#include "loguru/loguru.hpp"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace deserialize {
				void Context::run()
				{
					if(!mData) {
						throw GradidoNullPointerException("mData is empty", "memory::ConstBlockPtr", "gradido::v3_3::interaction_deserialize::Context::run");
					}
					if (Type::TRANSACTION_BODY == mType || Type::UNKNOWN == mType) {
						try {
							auto [body, bufferEnd2] = message_coder<TransactionBodyMessage>::decode(mData->span());
							mTransactionBody = std::move(TransactionBodyRole(body).getTransactionBody());
							mType = Type::TRANSACTION_BODY;
							return;
						}
						catch (std::exception& ex) {
							if (Type::TRANSACTION_BODY == mType) {
								LOG_F(WARNING, "couldn't deserialize, maybe wrong type? exception: %s", ex.what());
							}
							mType = Type::UNKNOWN;
						}
					}
					if (Type::GRADIDO_TRANSACTION == mType || Type::UNKNOWN == mType) {
						try {
							auto [gradidoTransaction, bufferEnd2] = message_coder<GradidoTransactionMessage>::decode(mData->span());
							mGradidoTransaction = std::move(GradidoTransactionRole(gradidoTransaction).getGradidoTransaction());
							mType = Type::GRADIDO_TRANSACTION;
							return;
						}
						catch (std::exception& ex) {
							if (Type::GRADIDO_TRANSACTION == mType) {
								LOG_F(WARNING, "couldn't deserialize, maybe wrong type? exception: %s", ex.what());
							}
							mType = Type::UNKNOWN;
						}
					}
					try {
						auto [confirmedTransaction, bufferEnd2] = message_coder<ConfirmedTransactionMessage>::decode(mData->span());
						mConfirmedTransaction = ConfirmedTransactionRole(confirmedTransaction).getConfirmedTransaction();
						mType = Type::CONFIRMED_TRANSACTION;
						return;
					}
					catch (std::exception& ex) {
						LOG_F(WARNING, "couldn't deserialize, invalid or unknown dataset! exception: %s", ex.what());
						mType = Type::UNKNOWN;
					}
				}
			}
		}
	}
}

