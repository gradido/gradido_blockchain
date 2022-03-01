#include "gradido_blockchain/model/protobufWrapper/GradidoBlock.h"
#include "gradido_blockchain/model/protobufWrapper/ProtobufExceptions.h"
#include "gradido_blockchain/model/protobufWrapper/TransactionValidationExceptions.h"
#include "gradido_blockchain/model/IGradidoBlockchain.h"

#include <google/protobuf/util/json_util.h>
#include "gradido/TransactionBody.pb.h"

#include "gradido_blockchain/lib/DataTypeConverter.h"
#include "gradido_blockchain/lib/Decay.h"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"



using namespace rapidjson;

namespace model {
	namespace gradido {
		
		GradidoBlock::GradidoBlock(const std::string* serializedGradidoBlock)
			: mGradidoTransaction(nullptr)
		{
			mProtoGradidoBlock = new proto::gradido::GradidoBlock;
			if (!mProtoGradidoBlock->ParseFromString(serializedGradidoBlock->data())) {
				throw ProtobufParseException(serializedGradidoBlock->data());
			}
			mGradidoTransaction = new GradidoTransaction(mProtoGradidoBlock->mutable_transaction());
		}

		GradidoBlock::GradidoBlock(std::unique_ptr<GradidoTransaction> transaction)
			: mGradidoTransaction(transaction.release())
		{
			mProtoGradidoBlock = new proto::gradido::GradidoBlock;
			mProtoGradidoBlock->set_allocated_transaction(mGradidoTransaction->getProto());
		}

		GradidoBlock::~GradidoBlock()
		{
			if (mGradidoTransaction) {
				delete mGradidoTransaction;
			}
			if (mProtoGradidoBlock) {
				delete mProtoGradidoBlock;
			}
		}

		Poco::SharedPtr<GradidoBlock> GradidoBlock::create(std::unique_ptr<GradidoTransaction> transaction, uint64_t id, int64_t received, const MemoryBin* messageId)
		{
			Poco::SharedPtr<GradidoBlock> gradidoBlock(new GradidoBlock(std::move(transaction)));
			auto proto = gradidoBlock->mProtoGradidoBlock;
			proto->set_id(id);
			proto->mutable_received()->set_seconds(received);
			proto->set_version_number(GRADIDO_BLOCK_PROTOCOL_VERSION);
			proto->set_allocated_message_id(messageId->copyAsString().release());
			return gradidoBlock;
		}

		std::string GradidoBlock::toJson()
		{
			std::string json_message = "";
			std::string json_message_body = "";
			google::protobuf::util::JsonPrintOptions options;
			options.add_whitespace = true;
			options.always_print_primitive_fields = true;

			auto protoGradidoBlock = *mProtoGradidoBlock;

			auto status = google::protobuf::util::MessageToJsonString(*mProtoGradidoBlock, &json_message, options);
			if (!status.ok()) {
				throw ProtobufJsonSerializationException("error parsing gradido block", *mProtoGradidoBlock, status);
			}

			proto::gradido::TransactionBody body;
			body.ParseFromString(mProtoGradidoBlock->transaction().body_bytes());
			status = google::protobuf::util::MessageToJsonString(body, &json_message_body, options);
			if (!status.ok()) {				
				throw ProtobufJsonSerializationException("error parsing transaction body", *mProtoGradidoBlock, status);
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

		std::unique_ptr<std::string> GradidoBlock::getSerialized()
		{
			mProtoGradidoBlock->mutable_transaction()->set_allocated_body_bytes(mGradidoTransaction->getTransactionBody()->getBodyBytes().release());
			//mProtoGradidoTransaction->set_allocated_body_bytes(mTransactionBody->getBodyBytes().release());

			auto size = mProtoGradidoBlock->ByteSizeLong();
			//auto bodyBytesSize = MemoryManager::getInstance()->getFreeMemory(mProtoCreation.ByteSizeLong());
			std::string* resultString(new std::string(size, 0));
			if (!mProtoGradidoBlock->SerializeToString(resultString)) {
				//addError(new Error("TransactionCreation::getBodyBytes", "error serializing string"));
				throw ProtobufSerializationException(*mProtoGradidoBlock);
			}
			std::unique_ptr<std::string> result;
			result.reset(resultString);
			return result;
		}

		MemoryBin* GradidoBlock::getMessageId() const
		{
			if (!mProtoGradidoBlock->message_id().size()) {
				return nullptr;
			}
			auto mm = MemoryManager::getInstance();
			auto result = mm->getMemory(mProtoGradidoBlock->message_id().size());
			memcpy(*result, mProtoGradidoBlock->message_id().data(), result->size());
			return result;
		}

		std::string GradidoBlock::getMessageIdHex() const 
		{
			return DataTypeConverter::binToHex(
				(const unsigned char*)mProtoGradidoBlock->message_id().data(),
				mProtoGradidoBlock->message_id().size()
			);
		}

		bool GradidoBlock::validate(
			TransactionValidationLevel level /*= TRANSACTION_VALIDATION_SINGLE*/,
			IGradidoBlockchain* blockchain /*= nullptr*/,
			IGradidoBlockchain* otherBlockchain /*= nullptr*/
		) const
		{
			if ((level & TRANSACTION_VALIDATION_SINGLE) == TRANSACTION_VALIDATION_SINGLE) {
				if (mProtoGradidoBlock->version_number() != GRADIDO_BLOCK_PROTOCOL_VERSION) {
					TransactionValidationInvalidInputException exception("wrong version in gradido block", "version_number", "uint64");
					exception.setTransactionBody(getGradidoTransaction()->getTransactionBody());
					throw exception;
				}
				if (!mProtoGradidoBlock->message_id().size()) {
					TransactionValidationInvalidInputException exception("empty", "message_id", "binary");
					exception.setTransactionBody(getGradidoTransaction()->getTransactionBody());
					throw exception;
				}
			}

			if ((level & TRANSACTION_VALIDATION_SINGLE_PREVIOUS) == TRANSACTION_VALIDATION_SINGLE_PREVIOUS) {
				if (getID() > 1) {
					assert(blockchain);
					auto previousBlock = blockchain->getTransactionForId(getID() - 1);
					if (previousBlock.isNull()) {
						GradidoBlockchainTransactionNotFoundException exception("previous transaction not found");
						throw exception.setTransactionId(getID() - 1);
					}
					auto previousGradidoBlock = std::make_unique<GradidoBlock>(previousBlock->getSerializedTransaction());
					if (previousGradidoBlock->getReceived() > getReceived()) {
						throw BlockchainOrderException("previous transaction is younger");						
					}
					auto mm = MemoryManager::getInstance();
					auto txHash = calculateTxHash(previousGradidoBlock.get());
					if (txHash->size() != mProtoGradidoBlock->running_hash().size()) {
						mm->releaseMemory(txHash);
						throw TransactionValidationException("tx hash size isn't equal");
					}
					if (0 != memcmp(*txHash, mProtoGradidoBlock->running_hash().data(), txHash->size())) {
						mm->releaseMemory(txHash);
						throw TransactionValidationInvalidInputException("stored tx hash isn't equal to calculated txHash", "txHash", "binary");
					}
					mm->releaseMemory(txHash);
				}
			}
			return mGradidoTransaction->validate(level, blockchain, this, otherBlockchain);
		}

		MemoryBin* GradidoBlock::calculateTxHash(const GradidoBlock* previousBlock) const
		{
			auto mm = MemoryManager::getInstance();
			std::string prevTxHash;
			if (previousBlock) {
				prevTxHash = previousBlock->getTxHash();
			}
			std::string transactionIdString = std::to_string(mProtoGradidoBlock->id());
			std::string receivedString;

			//yyyy-MM-dd HH:mm:ss
			
			Poco::DateTime received(Poco::Timestamp(mProtoGradidoBlock->received().seconds() * Poco::Timestamp::resolution()));
			receivedString = Poco::DateTimeFormatter::format(received, "%Y-%m-%d %H:%M:%S");
			std::string signatureMapString = mProtoGradidoBlock->transaction().sig_map().SerializeAsString();
			uint64_t finalGdd = mProtoGradidoBlock->final_gdd();

			auto hash = mm->getMemory(crypto_generichash_BYTES);

			// Sodium use for the generichash function BLAKE2b today (11.11.2019), mabye change in the future
			crypto_generichash_state state;
			crypto_generichash_init(&state, nullptr, 0, crypto_generichash_BYTES);
			if (prevTxHash.size()) {
				auto prexHashHex = DataTypeConverter::binToHex(prevTxHash);
				printf("[GradidoBlock::calculateTxHash] calculate with prev tx hash: %s\n", prexHashHex.data());
				crypto_generichash_update(&state, (const unsigned char*)prevTxHash.data(), prevTxHash.size());
			}
			crypto_generichash_update(&state, (const unsigned char*)transactionIdString.data(), transactionIdString.size());
			//printf("transaction id string: %s\n", transactionIdString.data());
			crypto_generichash_update(&state, (const unsigned char*)receivedString.data(), receivedString.size());
			//printf("received: %s\n", receivedString.data());
			crypto_generichash_update(&state, (const unsigned char*)signatureMapString.data(), signatureMapString.size());
			//printf("signature map serialized: %s\n", convertBinToHex(signatureMapString).data());
			crypto_generichash_update(&state, (const unsigned char*)&finalGdd, sizeof(uint64_t));
			crypto_generichash_final(&state, *hash, hash->size());
			return hash;
		}

		void GradidoBlock::calculateFinalGDD(Poco::SharedPtr<GradidoBlock> lastFinalBlock, std::vector<std::pair<int64_t, Poco::DateTime>> amountRetrievedSinceLastFinalBlock)
		{
			assert(!lastFinalBlock.isNull());
			std::unique_lock _lock(mWorkMutex);
			// get coin color
			// search for last transaction for this address with the same coin color
			// must be done before this function call
			// take last value + decay until this block
			
			mpfr_t temp, gradido_decimal, decay_for_duration;
			mpz_t gdd_cent;
			auto lastDate = lastFinalBlock->getReceived();

			mpfr_init(temp); mpfr_init(gradido_decimal); mpfr_init(decay_for_duration);
			mpz_init(gdd_cent);
			mpfr_set_si(gradido_decimal, 0, MPFR_RNDN);
			//mpz_set_sll(gdd_cent, input.gradido);
			mpz_set_si(gdd_cent, lastFinalBlock->getFinalBalance());

			for (auto it = amountRetrievedSinceLastFinalBlock.begin(); it != amountRetrievedSinceLastFinalBlock.end(); it++) {
				assert(it->second > lastDate);
				calculateDecayFactorForDuration(decay_for_duration, gDecayFactor356Days, Poco::Timespan(it->second - lastDate).totalSeconds());
				calculateDecayFast(decay_for_duration, gradido_decimal, gdd_cent, temp);
				mpz_add_ui(gdd_cent, gdd_cent, it->first);
				lastDate = it->second;
			}
			assert(getReceived() > lastDate);
			calculateDecayFactorForDuration(decay_for_duration, gDecayFactor356Days, Poco::Timespan(getReceived() - lastDate).totalSeconds());
			calculateDecayFast(decay_for_duration, gradido_decimal, gdd_cent, temp);
			auto newFinalBalance = mpz_get_si(gdd_cent);

			mpfr_clear(temp); mpfr_clear(gradido_decimal); mpfr_clear(decay_for_duration);
			mpz_clear(gdd_cent);
			
			// add value from this block if it was a transfer or creation transaction
			auto transactionBody = mGradidoTransaction->getTransactionBody();
			auto transactionType = transactionBody->getTransactionType();

			if (TRANSACTION_CREATION == transactionType) {
				newFinalBalance += transactionBody->getCreationTransaction()->getAmount();
			}
			else if (TRANSACTION_TRANSFER == transactionType || TRANSACTION_DEFERRED_TRANSFER == transactionType) {
				// if it is a transfer transaction this address must be the sender
				newFinalBalance -= transactionBody->getTransferTransaction()->getAmount();
			}
			mProtoGradidoBlock->set_final_gdd(newFinalBalance);
			
		}

	}
}