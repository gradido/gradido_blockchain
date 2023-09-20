#include "gradido_blockchain/model/protobufWrapper/ConfirmedTransaction.h"
#include "gradido_blockchain/model/protobufWrapper/ProtobufExceptions.h"
#include "gradido_blockchain/model/protobufWrapper/TransactionValidationExceptions.h"
#include "gradido_blockchain/model/IGradidoBlockchain.h"


#include <google/protobuf/util/json_util.h>
#include "proto/gradido/transaction_body.pb.h"

#include "gradido_blockchain/lib/DataTypeConverter.h"
#include "gradido_blockchain/lib/Decay.h"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"

using namespace rapidjson;

namespace model {
	namespace gradido {

		ConfirmedTransaction::ConfirmedTransaction(const std::string* serializedGradidoBlock)
			: mGradidoTransaction(nullptr)
		{
			assert(serializedGradidoBlock);
			mProtobufArenaMemory = ProtobufArenaMemory::create();
			mProtoConfirmedTransaction = google::protobuf::Arena::CreateMessage<proto::gradido::ConfirmedTransaction>(*mProtobufArenaMemory);

			if (!mProtoConfirmedTransaction->ParseFromArray(serializedGradidoBlock->data(), serializedGradidoBlock->size())) {
				throw ProtobufParseException(serializedGradidoBlock->data());
			}
			mGradidoTransaction = new GradidoTransaction(mProtoConfirmedTransaction->mutable_transaction(), mProtobufArenaMemory);
		}

		ConfirmedTransaction::ConfirmedTransaction(std::unique_ptr<GradidoTransaction> transaction)
			: mGradidoTransaction(transaction.release())
		{
			assert(mGradidoTransaction);
			mProtobufArenaMemory = mGradidoTransaction->getProtobufArena();
			mProtoConfirmedTransaction = google::protobuf::Arena::CreateMessage<proto::gradido::ConfirmedTransaction>(*mProtobufArenaMemory);
			auto gradidoTransactionProto = mGradidoTransaction->getProto();
			mProtoConfirmedTransaction->unsafe_arena_set_allocated_transaction(mGradidoTransaction->getProto());
		}

		ConfirmedTransaction::~ConfirmedTransaction()
		{
			if (mGradidoTransaction) {
				delete mGradidoTransaction;
				mGradidoTransaction = nullptr;
			}
			// proto memory will be released automatic with arena memory
			mProtoConfirmedTransaction = nullptr;
		}

		Poco::SharedPtr<ConfirmedTransaction> ConfirmedTransaction::create(std::unique_ptr<GradidoTransaction> transaction, uint64_t id, int64_t received, const MemoryBin* messageId)
		{
			Poco::SharedPtr<ConfirmedTransaction> confirmedTransaction(new ConfirmedTransaction(std::move(transaction)));
			auto proto = confirmedTransaction->mProtoConfirmedTransaction;
			proto->set_id(id);
			proto->mutable_confirmed_at()->set_seconds(received);
			proto->set_version_number(GRADIDO_BLOCK_PROTOCOL_VERSION);
			if (messageId) {
				proto->set_allocated_message_id(messageId->copyAsString().release());
			}
			return confirmedTransaction;
		}

		std::string ConfirmedTransaction::toJson()
		{
			std::string json_message = "";
			std::string json_message_body = "";
			google::protobuf::util::JsonPrintOptions options;
			options.add_whitespace = true;
			options.always_print_primitive_fields = true;

			auto status = google::protobuf::util::MessageToJsonString(*mProtoConfirmedTransaction, &json_message, options);
			if (!status.ok()) {
				throw ProtobufJsonSerializationException("error parsing gradido block", *mProtoConfirmedTransaction, status);
			}

			proto::gradido::TransactionBody body;
			body.ParseFromString(mProtoConfirmedTransaction->transaction().body_bytes());
			status = google::protobuf::util::MessageToJsonString(body, &json_message_body, options);
			if (!status.ok()) {
				throw ProtobufJsonSerializationException("error parsing transaction body", *mProtoConfirmedTransaction, status);
			}
			//\"bodyBytes\": \"MigKIC7Sihz14RbYNhVAa8V3FSIhwvd0pWVvZqDnVA91dtcbIgRnZGQx\"
			int startBodyBytes = json_message.find("bodyBytes") + std::string("\"bodyBytes\": \"").size() - 2;
			int endCur = json_message.find_first_of('\"', startBodyBytes + 2) + 1;
			json_message.replace(startBodyBytes, endCur - startBodyBytes, json_message_body);
			//printf("json: %s\n", json_message.data());


			Document parsed_json;
			parsed_json.Parse(json_message.data(), json_message.size());
			if (parsed_json.HasParseError()) {
				throw RapidjsonParseErrorException("error parsing json", parsed_json.GetParseError(), parsed_json.GetErrorOffset());
			}
			else {
				if (DataTypeConverter::replaceBase64WithHex(parsed_json, parsed_json.GetAllocator())) {
					StringBuffer buffer;
					PrettyWriter<StringBuffer> writer(buffer);
					parsed_json.Accept(writer);

					json_message = std::string(buffer.GetString(), buffer.GetLength());
				}
			}

			return json_message;
		}

		Value ConfirmedTransaction::toJson(Document& baseDocument)
		{
			auto jsonString = toJson();
			Document parsedJsonString;
			parsedJsonString.Parse(jsonString.data());
			auto alloc = baseDocument.GetAllocator();
			return Value(parsedJsonString, alloc);
		}

		uint64_t ConfirmedTransaction::getID() const
		{
			return mProtoConfirmedTransaction->id();
		}
		const std::string& ConfirmedTransaction::getTxHash() const
		{
			return mProtoConfirmedTransaction->running_hash();
		}
		const std::string& ConfirmedTransaction::getFinalBalance() const
		{
			return mProtoConfirmedTransaction->account_balance();
		}

		void ConfirmedTransaction::setTxHash(const MemoryBin* txHash)
		{
			mProtoConfirmedTransaction->set_allocated_running_hash(txHash->copyAsString().release());
		}

		int64_t ConfirmedTransaction::getConfirmedAt() const
		{
			return mProtoConfirmedTransaction->confirmed_at().seconds();
		}

		std::unique_ptr<std::string> ConfirmedTransaction::getSerialized()
		{
			mProtoConfirmedTransaction->mutable_transaction()->set_allocated_body_bytes(mGradidoTransaction->getTransactionBody()->getBodyBytes().release());
			//mProtoGradidoTransaction->set_allocated_body_bytes(mTransactionBody->getBodyBytes().release());

			auto size = mProtoConfirmedTransaction->ByteSizeLong();
			//auto bodyBytesSize = MemoryManager::getInstance()->getFreeMemory(mProtoCreation.ByteSizeLong());
			std::string* resultString(new std::string(size, 0));
			if (!mProtoConfirmedTransaction->SerializeToString(resultString)) {
				//addError(new Error("TransactionCreation::getBodyBytes", "error serializing string"));
				throw ProtobufSerializationException(mProtoConfirmedTransaction);
			}
			std::unique_ptr<std::string> result;
			result.reset(resultString);
			return result;
		}

		MemoryBin* ConfirmedTransaction::getMessageId() const
		{
			if (!mProtoConfirmedTransaction->message_id().size()) {
				return nullptr;
			}
			auto mm = MemoryManager::getInstance();
			auto result = mm->getMemory(mProtoConfirmedTransaction->message_id().size());
			memcpy(*result, mProtoConfirmedTransaction->message_id().data(), result->size());
			return result;
		}

		const std::string ConfirmedTransaction::getMessageIdString() const
		{
			return mProtoConfirmedTransaction->message_id();
		}

		std::string ConfirmedTransaction::getMessageIdHex() const
		{
			if (!mProtoConfirmedTransaction->message_id().size()) return "";
			return DataTypeConverter::binToHex(
				(const unsigned char*)mProtoConfirmedTransaction->message_id().data(),
				mProtoConfirmedTransaction->message_id().size()
			);
		}

		bool ConfirmedTransaction::validate(
			TransactionValidationLevel level /*= TRANSACTION_VALIDATION_SINGLE*/,
			IGradidoBlockchain* blockchain /*= nullptr*/,
			IGradidoBlockchain* otherBlockchain /*= nullptr*/
		) const
		{
			auto created = Poco::Timestamp(getGradidoTransaction()->getTransactionBody()->getCreatedSeconds() * Poco::Timestamp::resolution());
			if ((level & TRANSACTION_VALIDATION_SINGLE) == TRANSACTION_VALIDATION_SINGLE) {
				if (mProtoConfirmedTransaction->version_number() != GRADIDO_BLOCK_PROTOCOL_VERSION) {
					TransactionValidationInvalidInputException exception("wrong version in gradido block", "version_number", "uint64");
					exception.setTransactionBody(getGradidoTransaction()->getTransactionBody());
					throw exception;
				}	// with iota it is a BLAKE2b-256 hash with 256 Bits or 32 Bytes
				if (mProtoConfirmedTransaction->message_id().size() && mProtoConfirmedTransaction->message_id().size() != 32) {
					TransactionValidationInvalidInputException exception("wrong size", "message_id", "binary");
					exception.setTransactionBody(getGradidoTransaction()->getTransactionBody());
					throw exception;
				}
				
				if (Poco::Timespan(getReceivedAsTimestamp() - created).totalSeconds() < 0) {
					TransactionValidationInvalidInputException exception("timespan between created and received are negative", "iota milestone timestamp", std::to_string(getReceivedAsTimestamp().epochTime()).data());
					exception.setTransactionBody(getGradidoTransaction()->getTransactionBody());
					throw exception;
				}
			}

			if ((level & TRANSACTION_VALIDATION_SINGLE_PREVIOUS) == TRANSACTION_VALIDATION_SINGLE_PREVIOUS) {
				if (getID() > 1) {
					assert(blockchain);
					auto previousTransaction = blockchain->getTransactionForId(getID() - 1);
					if (previousTransaction.isNull()) {
						GradidoBlockchainTransactionNotFoundException exception("previous transaction not found");
						throw exception.setTransactionId(getID() - 1);
					}
					auto previousConfirmedTransaction = std::make_unique<ConfirmedTransaction>(previousTransaction->getSerializedTransaction());
					if (previousConfirmedTransaction->getConfirmedAt() > getConfirmedAt()) {
						throw BlockchainOrderException("previous transaction is younger");
					}
					auto previousCreated = Poco::Timestamp(previousConfirmedTransaction->getGradidoTransaction()->getTransactionBody()->getCreatedSeconds() * Poco::Timestamp::resolution());
					if (previousCreated > created) {
						auto timespanBetweenCreatedAndReceivedSeconds = Poco::Timespan(getReceivedAsTimestamp() - created).totalSeconds();
						if (timespanBetweenCreatedAndReceivedSeconds / 60 > MAGIC_NUMBER_MAX_TIMESPAN_BETWEEN_CREATING_AND_RECEIVING_TRANSACTION_IN_MINUTES) {
							TransactionValidationInvalidInputException exception("timespan between created and received are more than 2 minutes", "received/iota milestone timestamp", "int64");
							exception.setTransactionBody(getGradidoTransaction()->getTransactionBody());
							throw exception;
						}
					}
					auto mm = MemoryManager::getInstance();
					auto txHash = calculateTxHash(previousConfirmedTransaction.get());
					if (txHash->size() != mProtoConfirmedTransaction->running_hash().size()) {
						mm->releaseMemory(txHash);
						throw TransactionValidationException("tx hash size isn't equal");
					}
					if (0 != memcmp(*txHash, mProtoConfirmedTransaction->running_hash().data(), txHash->size())) {
						mm->releaseMemory(txHash);
						throw TransactionValidationInvalidInputException("stored tx hash isn't equal to calculated txHash", "txHash", "binary");
					}
					mm->releaseMemory(txHash);
				}
			}
			return mGradidoTransaction->validate(level, blockchain, this, otherBlockchain);
		}

		MemoryBin* ConfirmedTransaction::calculateTxHash(const ConfirmedTransaction* previousBlock) const
		{
			auto mm = MemoryManager::getInstance();
			std::string prevTxHash;
			if (previousBlock) {
				prevTxHash = previousBlock->getTxHash();
			}
			std::string transactionIdString = std::to_string(mProtoConfirmedTransaction->id());
			std::string receivedString;

			//yyyy-MM-dd HH:mm:ss

			Poco::DateTime received(Poco::Timestamp(mProtoConfirmedTransaction->confirmed_at().seconds() * Poco::Timestamp::resolution()));
			receivedString = Poco::DateTimeFormatter::format(received, "%Y-%m-%d %H:%M:%S");
			std::string signatureMapString = mProtoConfirmedTransaction->transaction().sig_map().SerializeAsString();

			auto hash = mm->getMemory(crypto_generichash_BYTES);

			// Sodium use for the generic hash function BLAKE2b today (11.11.2019), maybe change in the future
			crypto_generichash_state state;
			crypto_generichash_init(&state, nullptr, 0, crypto_generichash_BYTES);
			if (prevTxHash.size()) {
				auto prexHashHex = DataTypeConverter::binToHex(prevTxHash);
				crypto_generichash_update(&state, (const unsigned char*)prevTxHash.data(), prevTxHash.size());
			}
			crypto_generichash_update(&state, (const unsigned char*)transactionIdString.data(), transactionIdString.size());
			//printf("transaction id string: %s\n", transactionIdString.data());
			crypto_generichash_update(&state, (const unsigned char*)receivedString.data(), receivedString.size());
			//printf("received: %s\n", receivedString.data());
			crypto_generichash_update(&state, (const unsigned char*)signatureMapString.data(), signatureMapString.size());
			//printf("signature map serialized: %s\n", convertBinToHex(signatureMapString).data());
			crypto_generichash_update(&state, (const unsigned char*)mProtoConfirmedTransaction->account_balance().data(), sizeof(uint64_t));
			crypto_generichash_final(&state, *hash, hash->size());
			return hash;
		}

		void ConfirmedTransaction::calculateAccountBalance(IGradidoBlockchain* blockchain)
		{
			assert(blockchain);
			std::unique_lock _lock(mWorkMutex);

			// get coin color
			// search for last transaction for this address with the same coin color
			// take last value + decay until this block

			auto mm = MemoryManager::getInstance();
			std::string address;
			auto transactionBody = getGradidoTransaction()->getTransactionBody();
			auto transactionType = transactionBody->getTransactionType();

			switch (transactionType) {
			case model::gradido::TRANSACTION_NONE: throw std::runtime_error("transaction with type none is invalid");
			case model::gradido::TRANSACTION_CREATION:
				address = transactionBody->getCreationTransaction()->getRecipientPublicKeyString();
				break;
			case model::gradido::TRANSACTION_TRANSFER:
			case model::gradido::TRANSACTION_DEFERRED_TRANSFER:
				address = transactionBody->getTransferTransaction()->getSenderPublicKeyString();
				break;
			case model::gradido::TRANSACTION_GROUP_FRIENDS_UPDATE:
				return;
			case model::gradido::TRANSACTION_REGISTER_ADDRESS:
				if (getGradidoTransaction()->getTransactionBody()->isLocal()) {
					mProtoConfirmedTransaction->set_account_balance("0");
				}
				else {
					// when moving user from one to another group
					assert(false || "not implemented yet");
				}
				return;
			default: throw GradidoUnknownEnumException("unknown enum", "model::gradido:TransactionType", (int)transactionBody->getTransactionType());
			}
			mpfr_ptr finalBalance;
			try {
				finalBalance = blockchain->calculateAddressBalance(address, TransactionEntry::getCoinGroupId(transactionBody), getReceivedAsTimestamp());
			}
			catch (Poco::NullPointerException& ex) {
				printf("Poco Null Pointer exception by calling calculateAddressBalance\n");
				throw;
			}
			auto temp = mm->getMathMemory();
			// add value from this block if it was a transfer or creation transaction

			if (TRANSACTION_CREATION == transactionType) {
				mpfr_set_str(temp, transactionBody->getCreationTransaction()->getAmount().data(), 10, gDefaultRound);
				mpfr_add(finalBalance, finalBalance, temp, gDefaultRound);
			}
			else if (TRANSACTION_TRANSFER == transactionType || TRANSACTION_DEFERRED_TRANSFER == transactionType) {
				// if it is a transfer transaction this address must be the sender
				mpfr_set_str(temp, transactionBody->getTransferTransaction()->getAmount().data(), 10, gDefaultRound);
				mpfr_sub(finalBalance, finalBalance, temp, gDefaultRound);
			}
			TransactionBase::amountToString(mProtoConfirmedTransaction->mutable_account_balance(), finalBalance);
			mm->releaseMathMemory(finalBalance);
			mm->releaseMathMemory(temp);
		}

	}
}
