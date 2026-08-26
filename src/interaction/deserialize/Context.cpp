#include "gradido_blockchain/data/TransactionBody.h"
#include "gradido_blockchain/data/GradidoTransaction.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/interaction/deserialize/ConfirmedTransactionRole.h"
#include "gradido_blockchain/interaction/deserialize/ConfirmedTransactionCompactRole.h"
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
				if (!mData) {
					throw GradidoNullPointerException("mData is empty", "memory::ConstBlockPtr", "gradido::interaction_deserialize::Context::run");
				}
								
				if (Type::TRANSACTION_TRIGGER_EVENT == mType || Type::UNKNOWN == mType) {
					try {
						auto result = message_coder<TransactionTriggerEventMessage>::decode(mData->span());
						if (!result.has_value()) throw GradidoNodeInvalidDataException("protopuf failed with deserialize");
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
						if (!result.has_value()) throw GradidoNodeInvalidDataException("protopuf failed with deserialize");;
						const auto& [hieroAccountId, bufferEnd2] = *result;
						mHieroAccountId = HieroAccountIdRole(hieroAccountId);
						mType = Type::HIERO_ACCOUNT_ID;
						return;
					}
					catch (std::exception& ex) {
						if (Type::HIERO_ACCOUNT_ID == mType) {
							LOG_F(WARNING, "couldn't deserialize as hiero account id, maybe wrong type? exception: %s", ex.what());
						}
						mType = Type::UNKNOWN;
					}
				}
				if (Type::HIERO_TOPIC_ID == mType || Type::UNKNOWN == mType) {
					try {
						auto result = message_coder<HieroTopicIdMessage>::decode(mData->span());
						if (!result.has_value()) throw GradidoNodeInvalidDataException("protopuf failed with deserialize");;
						const auto& [hieroTopicId, bufferEnd2] = *result;
						mHieroTopicId = HieroTopicIdRole(hieroTopicId);
						mType = Type::HIERO_TOPIC_ID;
						return;
					}
					catch (std::exception& ex) {
						if (Type::HIERO_TOPIC_ID == mType) {
							LOG_F(WARNING, "couldn't deserialize as hiero topic id, maybe wrong type? exception: %s", ex.what());
						}
						mType = Type::UNKNOWN;
					}
				}
				if (Type::HIERO_TRANSACTION_ID == mType || Type::UNKNOWN == mType) {
					try {
						auto result = message_coder<HieroTransactionIdMessage>::decode(mData->span());
						if (!result.has_value()) throw GradidoNodeInvalidDataException("protopuf failed with deserialize");;
						const auto& [hieroTransactionId, bufferEnd2] = *result;
						mHieroTransactionId = HieroTransactionIdRole(hieroTransactionId);
						mType = Type::HIERO_TRANSACTION_ID;
						return;
					}
					catch (std::exception& ex) {
						if (Type::HIERO_TRANSACTION_ID == mType) {
							LOG_F(WARNING, "couldn't deserialize as hiero transaction id, maybe wrong type? exception: %s", ex.what());
						}
						mType = Type::UNKNOWN;
					}
				}
				if (Type::LEDGER_ANCHOR == mType) {
					try {
						auto result = message_coder<LedgerAnchorMessage>::decode(mData->span());
						if (!result.has_value()) throw GradidoNodeInvalidDataException("protopuf failed with deserialize");;
						const auto& [ledgerAnchor, bufferEnd2] = *result;
						mLedgerAnchor = LedgerAnchorRole(ledgerAnchor);
						mType = Type::LEDGER_ANCHOR;
						return;
					}
					catch (std::exception& ex) {
						LOG_F(WARNING, "couldn't deserialize as ledger anchor, maybe wrong type? exception: %s", ex.what());
						mType = Type::UNKNOWN;
					}
				}
				LOG_F(WARNING, "couldn't find correct type, maybe you need call the other run function with communityIdIndex as parameter");
			}

			void Context::run(uint32_t communityIdIndex)
			{
				// TODO: shorten code with help of template
				if(!mData) {
					throw GradidoNullPointerException("mData is empty", "memory::ConstBlockPtr", "gradido::interaction_deserialize::Context::run");
				}
				if (Type::CONFIRMED_TRANSACTION_COMPACT == mType) {
					try {
						auto role = ConfirmedTransactionCompactRole(mData);
						role.run(communityIdIndex);
						mConfirmedTransactionCompact = role.getTransaction();
						mType = Type::CONFIRMED_TRANSACTION_COMPACT;
						return;
					}
					catch (std::exception& ex) {
						LOG_F(WARNING, "couldn't deserialize as gradido transaction, maybe wrong type? exception: %s", ex.what());
						mType = Type::UNKNOWN;
					}
				}
				if (Type::GRADIDO_TRANSACTION == mType || Type::UNKNOWN == mType) {
					try {
						auto role = GradidoTransactionRole(mData);
						role.run(communityIdIndex);
						mGradidoTransaction = role.getTransaction();
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
				if (Type::TRANSACTION_BODY == mType || Type::UNKNOWN == mType) {
					try {
						auto role = TransactionBodyRole(mData);
						role.run(communityIdIndex);
						mTransactionBody = role.getBody();
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
				
				try {
					auto role = ConfirmedTransactionRole(mData);
					role.run(communityIdIndex);
					mConfirmedTransaction = role.getTransaction();
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

