#include "gradido_blockchain/const.h"
#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/data/BalanceDerivationType.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/interaction/validate/ConfirmedTransactionRole.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/interaction/validate/GradidoTransactionRole.h"
#include "gradido_blockchain/interaction/deserialize/Context.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

#include "magic_enum/magic_enum.hpp"

using namespace std::chrono;
using namespace magic_enum;

namespace gradido {
	using data::BalanceDerivationType;
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
					if (mConfirmedTransaction.getLedgerAnchor().empty()) {
						TransactionValidationInvalidInputException exception(
							"invalid",
							"ledger_anchor",
							"LedgerAnchor",
							"iotaMessageId || hieroTransactionId || legacyTransactionId || nodeTriggeredTransactionId",
							nullptr
						);
						exception.setTransactionBody(*body);
						throw exception;
					}
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
						const auto& ledgerAnchor = mConfirmedTransaction.getLedgerAnchor();
						const auto& previousLedgerAnchor = previousConfirmedTransaction->getLedgerAnchor();
						if ((previousLedgerAnchor.isHieroTransactionId() || previousLedgerAnchor.isIotaMessageId())
							&& ledgerAnchor.isLegacyGradidoDbTransactionId()) {
							throw TransactionValidationInvalidInputException(
								"current transaction was imported from db while last transaction was confirmed by ledger",
								"ledger_anchor",
								"LedgerAnchor",
								magic_enum::enum_name(ledgerAnchor.getType()).data(),
								magic_enum::enum_name(previousLedgerAnchor.getType()).data()
							);
						}
					}
				}
				auto modifiedType = type;
				if (
					BalanceDerivationType::EXTERN == mConfirmedTransaction.getBalanceDerivationType()
					&& (modifiedType & Type::PREVIOUS_BALANCE) == Type::PREVIOUS_BALANCE
				) {
					modifiedType = modifiedType - Type::PREVIOUS_BALANCE;
				}
				GradidoTransactionRole(*mConfirmedTransaction.getGradidoTransaction()).run(
					modifiedType,
					blockchain,
					senderPreviousConfirmedTransaction,
					recipientPreviousConfirmedTransaction
				);
			}
		}
	}
}