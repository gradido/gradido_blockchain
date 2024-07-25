#include "gradido_blockchain/interaction/validate/ConfirmedTransactionRole.h"
#include "gradido_blockchain/const.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/interaction/validate/GradidoTransactionRole.h"

using namespace std::chrono;

namespace gradido {
	namespace interaction {
		namespace validate {
			void ConfirmedTransactionRole::run(
				Type type,
				std::string_view communityId,
				blockchain::AbstractProvider* blockchainProvider,
				data::ConstConfirmedTransactionPtr senderPreviousConfirmedTransaction,
				data::ConstConfirmedTransactionPtr recipientPreviousConfirmedTransaction
			) {
				auto body = mConfirmedTransaction.gradidoTransaction->getTransactionBody();
				auto createdAt = mConfirmedTransaction.gradidoTransaction->getTransactionBody()->createdAt.getAsTimepoint();
				auto confirmedAt = mConfirmedTransaction.confirmedAt.getAsTimepoint();

				if ((type & Type::SINGLE) == Type::SINGLE) {
					if (mConfirmedTransaction.versionNumber != GRADIDO_CONFIRMED_TRANSACTION_V3_3_VERSION_STRING) {
						TransactionValidationInvalidInputException exception("wrong version in gradido block", "version_number", "uint64");
						exception.setTransactionBody(*body);
						throw exception;
					}	// with iota it is a BLAKE2b-256 hash with 256 Bits or 32 Bytes
					if (mConfirmedTransaction.messageId && mConfirmedTransaction.messageId->size() != 32) {
						TransactionValidationInvalidInputException exception("wrong size", "message_id", "binary");
						exception.setTransactionBody(*body);
						throw exception;
					}						
						
					if (confirmedAt - createdAt < duration<int>::zero()) {
						TransactionValidationInvalidInputException exception(
							"timespan between created and received are negative",
							"iota milestone timestamp",
							std::to_string(mConfirmedTransaction.confirmedAt.seconds).data()
						);
						exception.setTransactionBody(*body);
						throw exception;
					}
				}

				if ((type & Type::PREVIOUS) == Type::PREVIOUS) {
					if (mConfirmedTransaction.id > 1) {
						auto previousTransactionId = mConfirmedTransaction.id - 1;
						auto blockchain = blockchainProvider->findBlockchain(communityId);
						assert(blockchain);
						auto previousTransaction = blockchain->getTransactionForId(previousTransactionId);
						if (!previousTransaction) {
							GradidoBlockchainTransactionNotFoundException exception("previous transaction not found");
							throw exception.setTransactionId(previousTransactionId);
						}
						auto previousConfirmedTransaction = previousTransaction->getConfirmedTransaction();
						if (previousConfirmedTransaction->confirmedAt > mConfirmedTransaction.confirmedAt) {
							throw BlockchainOrderException("previous transaction is younger");
						}
						auto previousCreated = previousConfirmedTransaction->gradidoTransaction->getTransactionBody()->createdAt.getAsTimepoint();
						// if previous transaction was created after this transaction we make sure that creation date and received/confirmation date are not 
						// to far apart
						// it is possible that they where created nearly at the same time but sorted from iota swapped
						if (previousCreated > createdAt) {
							auto timespanBetweenCreatedAndReceived = duration_cast<seconds>(confirmedAt - createdAt);
							if (timespanBetweenCreatedAndReceived > MAGIC_NUMBER_MAX_TIMESPAN_BETWEEN_CREATING_AND_RECEIVING_TRANSACTION) {
								TransactionValidationInvalidInputException exception(
									"timespan between created and received are more than 2 minutes",
									"received/iota milestone timestamp",
									"int64"
								);
								exception.setTransactionBody(*body);
								throw exception;
							}
						}
						auto txHash = mConfirmedTransaction.calculateRunningHash(previousConfirmedTransaction);
						if (!mConfirmedTransaction.runningHash || txHash.size() != mConfirmedTransaction.runningHash->size()) {
							throw TransactionValidationException("tx hash size isn't equal");
						}
						if(!txHash.isTheSame(mConfirmedTransaction.runningHash)) {
							throw TransactionValidationInvalidInputException("stored tx hash isn't equal to calculated txHash", "txHash", "binary");
						}
					}
				}
				GradidoTransactionRole(*mConfirmedTransaction.gradidoTransaction).run(
					type,
					communityId,
					blockchainProvider,
					senderPreviousConfirmedTransaction,
					recipientPreviousConfirmedTransaction
				);
			}
		}
	}
}