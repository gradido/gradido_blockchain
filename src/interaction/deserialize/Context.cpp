#include "gradido_blockchain/data/TransactionBody.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/interaction/deserialize/ConfirmedTransactionRole.h"
#include "gradido_blockchain/interaction/deserialize/Context.h"
#include "gradido_blockchain/interaction/deserialize/GradidoTransactionRole.h"
#include "gradido_blockchain/interaction/deserialize/TransactionBodyRole.h"
#include "gradido_blockchain/interaction/deserialize/TransactionTriggerEventRole.h"


#include "loguru/loguru.hpp"

namespace gradido {
	namespace interaction {
		namespace deserialize {
			void Context::run()
			{
				if(!mData) {
					throw GradidoNullPointerException("mData is empty", "memory::ConstBlockPtr", "gradido::interaction_deserialize::Context::run");
				}
				if (Type::TRANSACTION_BODY == mType || Type::UNKNOWN == mType) {
					try {
						auto result = message_coder<TransactionBodyMessage>::decode(mData->span());
						if (!result.has_value()) return;
						const auto& [body, bufferEnd2] = *result;
						mTransactionBody = std::make_shared<data::TransactionBody>(TransactionBodyRole(body).getBody());
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
						auto result = message_coder<GradidoTransactionMessage>::decode(mData->span());
						if (!result.has_value()) return;
						const auto& [gradidoTransaction, bufferEnd2] = *result;
						mGradidoTransaction = GradidoTransactionRole(gradidoTransaction).getGradidoTransaction();
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
				if (Type::TRANSACTION_TRIGGER_EVENT == mType || Type::UNKNOWN == mType) {
					try {
						auto result = message_coder<TransactionTriggerEventMessage>::decode(mData->span());
						if (!result.has_value()) return;
						const auto& [transactionTriggerEvent, bufferEnd2] = *result;
						mTransactionTriggerEvent = TransactionTriggerEventRole(transactionTriggerEvent);
						mType = Type::TRANSACTION_TRIGGER_EVENT;
						return;
					}
					catch (std::exception& ex) {
						if (Type::TRANSACTION_TRIGGER_EVENT == mType) {
							LOG_F(WARNING, "couldn't deserialize, maybe wrong type? exception: %s", ex.what());
						}
						mType = Type::UNKNOWN;
					}
				}
				try {
					auto result = message_coder<ConfirmedTransactionMessage>::decode(mData->span());
					if (!result.has_value()) return;
					const auto& [confirmedTransaction, bufferEnd2] = *result;
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

