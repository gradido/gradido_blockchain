#include "gradido_blockchain/const.h"
#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/interaction/validate/ConfirmedTransactionRole.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/interaction/validate/GradidoTransactionRole.h"
#include "gradido_blockchain/interaction/deserialize/Context.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

using namespace std::chrono;

namespace gradido {
	namespace interaction {
		namespace validate {
			void ConfirmedTransactionRole::run(
				Type type,
				std::shared_ptr<blockchain::Abstract> blockchain,
				std::shared_ptr<const data::ConfirmedTransaction> senderPreviousConfirmedTransaction,
				std::shared_ptr<const data::ConfirmedTransaction> recipientPreviousConfirmedTransaction
			) {
				auto body = mConfirmedTransaction.getGradidoTransaction()->getTransactionBody();
				auto createdAt = mConfirmedTransaction.getGradidoTransaction()->getTransactionBody()->getCreatedAt().getAsTimepoint();
				auto confirmedAt = mConfirmedTransaction.getConfirmedAt().getAsTimepoint();

				if ((type & Type::SINGLE) == Type::SINGLE) {
					if (mConfirmedTransaction.getVersionNumber() != GRADIDO_CONFIRMED_TRANSACTION_VERSION_STRING) {
						TransactionValidationInvalidInputException exception(
							"wrong version",
							"version_number",
							"string",
							GRADIDO_CONFIRMED_TRANSACTION_VERSION_STRING,
							mConfirmedTransaction.getVersionNumber().data()
						);
						exception.setTransactionBody(*body);
						throw exception;
					}	
					auto messageId = mConfirmedTransaction.getMessageId();
					if (messageId) {
						deserialize::Context deserializeHieroTransactionId(messageId, deserialize::Type::HIERO_TRANSACTION_ID);
						deserializeHieroTransactionId.run();
						if (!deserializeHieroTransactionId.isHieroTransactionId() || deserializeHieroTransactionId.getHieroTransactionId().empty()) {
							TransactionValidationInvalidInputException exception(
								"invalid",
								"message_id",
								"bytes",
								"hiero transaction id",
								messageId->convertToHex().data()
							);
							exception.setTransactionBody(*body);
							throw exception;
						}
					}
					// with iota it is a BLAKE2b-256 hash with 256 Bits or 32 Bytes					
					/*
					if (messageId && messageId->size() != 32) {
						TransactionValidationInvalidInputException exception(
							"wrong size",
							"message_id",
							"bytes",
							"32",
							std::to_string(messageId->size()).data()
						);
						exception.setTransactionBody(*body);
						throw exception;
					}*/						
					// I don't know how far a server clock can be off, but let simply ignore this for now and test this some time
					/*
					if (confirmedAt - createdAt < duration<int>::zero()) {
						std::string expected = ">= " + DataTypeConverter::timePointToString(createdAt);
						TransactionValidationInvalidInputException exception(
							"timespan between created and received are negative",
							"confirmed_at",
							"TimestampSeconds",
							expected.data(),
							DataTypeConverter::timePointToString(confirmedAt).data()
						);
						exception.setTransactionBody(*body);
						throw exception;
					}*/
				}

				if ((type & Type::PREVIOUS) == Type::PREVIOUS) {
					if (mConfirmedTransaction.getId() > 1) {
						auto previousTransactionId = mConfirmedTransaction.getId() - 1;
						auto previousTransaction = blockchain->getTransactionForId(previousTransactionId);
						if (!previousTransaction) {
							GradidoBlockchainTransactionNotFoundException exception("previous transaction not found");
							throw exception.setTransactionId(previousTransactionId);
						}
						auto previousConfirmedTransaction = previousTransaction->getConfirmedTransaction();
						if (previousConfirmedTransaction->getConfirmedAt() > mConfirmedTransaction.getConfirmedAt()) {
							throw BlockchainOrderException("previous transaction is younger");
						}
						
						if (confirmedAt - createdAt > MAGIC_NUMBER_MAX_TIMESPAN_BETWEEN_CREATING_AND_RECEIVING_TRANSACTION) {
							std::string message = "timespan between created and received are more than " + DataTypeConverter::timespanToString(MAGIC_NUMBER_MAX_TIMESPAN_BETWEEN_CREATING_AND_RECEIVING_TRANSACTION);
							std::string expected = "<= (" + DataTypeConverter::timePointToString(createdAt) + " + " + DataTypeConverter::timespanToString(MAGIC_NUMBER_MAX_TIMESPAN_BETWEEN_CREATING_AND_RECEIVING_TRANSACTION) + ")";
							TransactionValidationInvalidInputException exception(
								message.data(),
								"confirmed_at",
								"TimestampSeconds",
								expected.data(),
								DataTypeConverter::timePointToString(confirmedAt).data()
							);
							exception.setTransactionBody(*body);
							throw exception;
						}
						auto runningHash = mConfirmedTransaction.calculateRunningHash(previousConfirmedTransaction);
						if (!mConfirmedTransaction.getRunningHash() || runningHash->size() != mConfirmedTransaction.getRunningHash()->size()) {
							std::string fieldTypeWithSize = "binary[" + std::to_string(crypto_generichash_BYTES) + "]";
							std::string actual = "0";
							if(mConfirmedTransaction.getRunningHash()) {
								actual = std::to_string(mConfirmedTransaction.getRunningHash()->size());
							}
							throw TransactionValidationInvalidInputException(
								"stored running hash size isn't equal to calculated running hash size",
								"running_hash",
								fieldTypeWithSize.data(),
								std::to_string(runningHash->size()).data(),
								actual.data()
							);
						}
						if(!runningHash->isTheSame(mConfirmedTransaction.getRunningHash())) {
							std::string fieldTypeWithSize = "binary[" + std::to_string(crypto_generichash_BYTES) + "]";
							std::string actual = "";
							if(mConfirmedTransaction.getRunningHash()) {
								actual = mConfirmedTransaction.getRunningHash()->convertToHex();
							}
							
							throw TransactionValidationInvalidInputException(
								"stored tx hash isn't equal to calculated txHash",
								"running_hash",
								fieldTypeWithSize.data(),
								runningHash->convertToHex().data(),
								actual.data()
							);
						}
					}
				}
				GradidoTransactionRole(*mConfirmedTransaction.getGradidoTransaction()).run(
					type,
					blockchain,
					senderPreviousConfirmedTransaction,
					recipientPreviousConfirmedTransaction
				);
			}
		}
	}
}