#include "gradido_blockchain/data/TransactionBody.h"
#include "gradido_blockchain/data/GradidoTransaction.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/interaction/deserialize/ConfirmedTransactionRole.h"
#include "gradido_blockchain/interaction/deserialize/Context.h"
#include "gradido_blockchain/interaction/deserialize/GradidoTransactionRole.h"
#include "gradido_blockchain/interaction/deserialize/HieroAccountIdRole.h"
#include "gradido_blockchain/interaction/deserialize/HieroTopicIdRole.h"
#include "gradido_blockchain/interaction/deserialize/HieroTransactionIdRole.h"
#include "gradido_blockchain/interaction/deserialize/LedgerAnchorRole.h"
#include "gradido_blockchain/interaction/deserialize/TransactionBodyRole.h"
#include "gradido_blockchain/interaction/deserialize/TransactionTriggerEventRole.h"


#include "loguru/loguru.hpp"

namespace gradido {
	namespace interaction {
		namespace deserialize {
			Context::~Context()
			{

			}

			void Context::run()
			{
				// TODO: shorten code with help of template
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
							LOG_F(WARNING, "couldn't deserialize as transaction body, maybe wrong type? exception: %s", ex.what());
						}
						mType = Type::UNKNOWN;
					}
				}
				if (Type::GRADIDO_TRANSACTION == mType || Type::UNKNOWN == mType) {
					try {
						auto result = message_coder<GradidoTransactionMessage>::decode(mData->span());
						if (!result.has_value()) return;
						const auto& [gradidoTransaction, bufferEnd2] = *result;
						mGradidoTransaction = std::move(GradidoTransactionRole(gradidoTransaction).getGradidoTransaction());
						mType = Type::GRADIDO_TRANSACTION;
						return;
					}
					catch (std::exception& ex) {
						if (Type::GRADIDO_TRANSACTION == mType) {
							LOG_F(WARNING, "couldn't deserialize as gradido transaction, maybe wrong type? exception: %s", ex.what());
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
							LOG_F(WARNING, "couldn't deserialize as transaction trigger event, maybe wrong type? exception: %s", ex.what());
						}
						mType = Type::UNKNOWN;
					}
				}
				if (Type::HIERO_ACCOUNT_ID == mType || Type::UNKNOWN == mType) {
					try {
						auto result = message_coder<HieroAccountIdMessage>::decode(mData->span());
						if (!result.has_value()) return;
						const auto& [hieroAccountId, bufferEnd2] = *result;
						mHieroAccountId = HieroAccountIdRole(hieroAccountId);
						return;
					}
					catch (std::exception& ex) {
						if (Type::HIERO_ACCOUNT_ID == mType) {
							LOG_F(WARNING, "couldn't deserialize as hiero account id, maybe wrong type? exception: %s", ex.what());
						}
					}
				}
				if (Type::HIERO_TOPIC_ID == mType || Type::UNKNOWN == mType) {
					try {
						auto result = message_coder<HieroTopicIdMessage>::decode(mData->span());
						if (!result.has_value()) return;
						const auto& [hieroTopicId, bufferEnd2] = *result;
						mHieroTopicId = HieroTopicIdRole(hieroTopicId);
						return;
					}
					catch (std::exception& ex) {
						if (Type::HIERO_TOPIC_ID == mType) {
							LOG_F(WARNING, "couldn't deserialize as hiero topic id, maybe wrong type? exception: %s", ex.what());
						}
					}
				}
				if (Type::HIERO_TRANSACTION_ID == mType || Type::UNKNOWN == mType) {
					try {
						auto result = message_coder<HieroTransactionIdMessage>::decode(mData->span());
						if (!result.has_value()) return;
						const auto& [hieroTransactionId, bufferEnd2] = *result;
						mHieroTransactionId = HieroTransactionIdRole(hieroTransactionId);
						return;
					} 
					catch (std::exception& ex) {
						if (Type::HIERO_TRANSACTION_ID == mType) {
							LOG_F(WARNING, "couldn't deserialize as hiero transaction id, maybe wrong type? exception: %s", ex.what());
						}
					}
				}
				if (Type::LEDGER_ANCHOR == mType) {
					try {
						auto result = message_coder<LedgerAnchorMessage>::decode(mData->span());
						if (!result.has_value()) return;
						const auto& [ledgerAnchor, bufferEnd2] = *result;
						mLedgerAnchor = LedgerAnchorRole(ledgerAnchor);
						return;
					}
					catch (std::exception& ex) {
						LOG_F(WARNING, "couldn't deserialize as ledger anchor, maybe wrong type? exception: %s", ex.what());
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

