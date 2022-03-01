#include "gradido_blockchain/model/protobufWrapper/GradidoTransaction.h"
#include "gradido_blockchain/model/protobufWrapper/TransactionValidationExceptions.h"
#include "gradido_blockchain/model/protobufWrapper/ProtobufExceptions.h"

#include "gradido_blockchain/crypto/KeyPairEd25519.h"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"

using namespace rapidjson;

namespace model {
	namespace gradido {
		GradidoTransaction::GradidoTransaction(proto::gradido::GradidoTransaction* protoGradidoTransaction)
			: mProtoGradidoTransaction(protoGradidoTransaction), mTransactionBody(nullptr)
		{
			mTransactionBody = TransactionBody::load(protoGradidoTransaction->body_bytes());
		}

		GradidoTransaction::GradidoTransaction(const std::string& serializedProtobuf)
		{
			mProtoGradidoTransaction = new proto::gradido::GradidoTransaction;
			if (!mProtoGradidoTransaction->ParseFromString(serializedProtobuf)) {
				throw ProtobufParseException(serializedProtobuf);
			}
			mTransactionBody = TransactionBody::load(mProtoGradidoTransaction->body_bytes());
		}

		GradidoTransaction::GradidoTransaction(model::gradido::TransactionBody* body)
		{
			assert(body);
			mProtoGradidoTransaction = new proto::gradido::GradidoTransaction;
			mTransactionBody = body;
		}

		GradidoTransaction::~GradidoTransaction()
		{
			if (mTransactionBody) {
				delete mTransactionBody;
			}
		}

		bool GradidoTransaction::validate(
			TransactionValidationLevel level/* = TRANSACTION_VALIDATION_SINGLE*/,
			IGradidoBlockchain* blockchain/* = nullptr*/,
			const GradidoBlock* parentGradidoBlock/* = nullptr*/,
			IGradidoBlockchain* otherBlockchain/* = nullptr*/
		) const
		{
			if ((level & TRANSACTION_VALIDATION_SINGLE) == TRANSACTION_VALIDATION_SINGLE) 
			{
				auto sig_map = mProtoGradidoTransaction->sig_map();

				// check if all signatures belong to current body bytes
				auto body_bytes = mProtoGradidoTransaction->body_bytes();
				for (auto it = sig_map.sigpair().begin(); it != sig_map.sigpair().end(); it++)
				{
					KeyPairEd25519 key_pair((const unsigned char*)it->pubkey().data());
					if (!key_pair.verify(body_bytes, it->signature())) {
						throw TransactionValidationInvalidSignatureException(
							"pubkey don't belong to bodybytes", it->pubkey(), it->signature(), body_bytes
						);
					}
				}
				
				// check for not allowed signatures
				mTransactionBody->getTransactionBase()->checkRequiredSignatures(&sig_map);
				return mTransactionBody->validate(level, blockchain, parentGradidoBlock);
			}
			 
			// must be implemented in gradido node server
			if ((level & TRANSACTION_VALIDATION_PAIRED) == TRANSACTION_VALIDATION_PAIRED)
			{
				auto transactionBody = getTransactionBody();
				switch (transactionBody->getCrossGroupType()) {
				case proto::gradido::TransactionBody_CrossGroupType_LOCAL: break; 
				case proto::gradido::TransactionBody_CrossGroupType_INBOUND: break;
				case proto::gradido::TransactionBody_CrossGroupType_OUTBOUND: break;
				case proto::gradido::TransactionBody_CrossGroupType_CROSS: break;
				}
				throw std::runtime_error("not implemented yet");
			}
			
			return true;

		}

		bool GradidoTransaction::addSign(const MemoryBin* pubkeyBin, const MemoryBin* signatureBin)
		{
			std::unique_ptr<std::string> bodyBytes;
			
			bodyBytes = mTransactionBody->getBodyBytes();	
			auto sigMap = mProtoGradidoTransaction->mutable_sig_map();

			// check if pubkey already exist
			for (auto it = sigMap->sigpair().begin(); it != sigMap->sigpair().end(); it++)
			{
				if (it->pubkey().size() != KeyPairEd25519::getPublicKeySize()) {
					throw TransactionValidationInvalidInputException("invalid size", "public key");
				}
				if (0 == memcmp(pubkeyBin, it->pubkey().data(), KeyPairEd25519::getPublicKeySize())) {
					throw TransactionValidationInvalidInputException("already exist", "public key");
				}
			}

			auto sigPair = sigMap->add_sigpair();
			auto pubkeyBytes = sigPair->mutable_pubkey();
			*pubkeyBytes = std::string((const char*)pubkeyBin, crypto_sign_PUBLICKEYBYTES);

			auto sigBytes = sigPair->mutable_signature();
			*sigBytes = std::string((const char*)signatureBin->data(), crypto_sign_BYTES);
			return sigMap->sigpair_size() >= mTransactionBody->getTransactionBase()->getMinSignatureCount();
			
		}

		MemoryBin* GradidoTransaction::getParentMessageId() const
		{
			auto mm = MemoryManager::getInstance();
			if (!mProtoGradidoTransaction->parent_message_id().size()) {
				return nullptr;
			}
			auto result = mm->getMemory(mProtoGradidoTransaction->parent_message_id().size());
			memcpy(*result, mProtoGradidoTransaction->parent_message_id().data(), result->size());
			return result;
		}

		std::unique_ptr<std::string> GradidoTransaction::getSerialized()
		{
			mProtoGradidoTransaction->set_allocated_body_bytes(mTransactionBody->getBodyBytes().release());

			auto size = mProtoGradidoTransaction->ByteSizeLong();
			//auto bodyBytesSize = MemoryManager::getInstance()->getFreeMemory(mProtoCreation.ByteSizeLong());
			std::string* resultString(new std::string(size, 0));
			if (!mProtoGradidoTransaction->SerializeToString(resultString)) {
				//addError(new Error("TransactionCreation::getBodyBytes", "error serializing string"));
				throw ProtobufSerializationException(*mProtoGradidoTransaction);
			}
			std::unique_ptr<std::string> result;
			result.reset(resultString);
			return result;
		}

		std::string GradidoTransaction::toJson() const
		{
			std::string json_message = "";
			std::string json_message_body = "";
			google::protobuf::util::JsonPrintOptions options;
			options.add_whitespace = true;
			options.always_print_primitive_fields = true;

			proto::gradido::TransactionBody body;
			body.ParseFromString(mProtoGradidoTransaction->body_bytes());
			auto status = google::protobuf::util::MessageToJsonString(body, &json_message_body, options);
			if (!status.ok()) {
				throw ProtobufJsonSerializationException("error parsing transaction body", *mProtoGradidoTransaction, status);
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
	}
}