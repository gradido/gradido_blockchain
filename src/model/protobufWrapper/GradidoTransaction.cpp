#include "gradido_blockchain/model/protobufWrapper/GradidoTransaction.h"
#include "gradido_blockchain/model/protobufWrapper/TransactionValidationExceptions.h"
#include "gradido_blockchain/model/protobufWrapper/ProtobufExceptions.h"
#include "gradido_blockchain/model/IGradidoBlockchain.h"

#include "gradido_blockchain/crypto/KeyPairEd25519.h"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"

using namespace rapidjson;

namespace model {
	namespace gradido {
		GradidoTransaction::GradidoTransaction(proto::gradido::GradidoTransaction* protoGradidoTransaction, std::shared_ptr<ProtobufArenaMemory> arenaMemory)
			: mProtoGradidoTransaction(protoGradidoTransaction), mTransactionBody(nullptr), mProtobufArenaMemory(arenaMemory), mBodyDirty(false)
		{
			mTransactionBody = TransactionBody::load(protoGradidoTransaction->body_bytes(), arenaMemory);
		}

		GradidoTransaction::GradidoTransaction(const std::string* serializedProtobuf)
			: mProtoGradidoTransaction(nullptr), mBodyDirty(false)
		{
			mProtobufArenaMemory = ProtobufArenaMemory::create();
			mProtoGradidoTransaction = google::protobuf::Arena::CreateMessage<proto::gradido::GradidoTransaction>(*mProtobufArenaMemory);
			if (!mProtoGradidoTransaction->ParseFromArray(serializedProtobuf->data(), serializedProtobuf->size())) {
				if (!mProtoGradidoTransaction->ParsePartialFromArray(serializedProtobuf->data(), serializedProtobuf->size())) {
					throw ProtobufParseException(*serializedProtobuf);
				}
				else {
					printf("only partial error\n");
					throw ProtobufParseException(*serializedProtobuf);
				}
			}
			mTransactionBody = TransactionBody::load(mProtoGradidoTransaction->body_bytes(), mProtobufArenaMemory);
		}

		GradidoTransaction::GradidoTransaction(const void *data, int size)
			: mProtoGradidoTransaction(nullptr), mBodyDirty(false)
		{
			mProtobufArenaMemory = ProtobufArenaMemory::create();
			mProtoGradidoTransaction = google::protobuf::Arena::CreateMessage<proto::gradido::GradidoTransaction>(*mProtobufArenaMemory);
			if (!mProtoGradidoTransaction->ParseFromArray(data, size))
			{
				std::string serializedProtobuf;
				serializedProtobuf.resize(size, 0);
				memcpy(serializedProtobuf.data(), data, size);
				if (!mProtoGradidoTransaction->ParsePartialFromArray(data, size)) {
					throw ProtobufParseException(serializedProtobuf);
				} 
				else {
					printf("only partial error\n");
					throw ProtobufParseException(serializedProtobuf);
				}
			}
			mTransactionBody = TransactionBody::load(mProtoGradidoTransaction->body_bytes(), mProtobufArenaMemory);
		}

		GradidoTransaction::GradidoTransaction(model::gradido::TransactionBody* body)
			: mProtoGradidoTransaction(nullptr), mBodyDirty(false)
		{
			assert(body);
			mTransactionBody = body;
			mProtobufArenaMemory = body->getProtobufArena();
			mProtoGradidoTransaction = google::protobuf::Arena::CreateMessage<proto::gradido::GradidoTransaction>(*mProtobufArenaMemory);
			mProtoGradidoTransaction->set_allocated_body_bytes(mTransactionBody->getBodyBytes().release());
		}

		GradidoTransaction::~GradidoTransaction()
		{
			if (mTransactionBody) {
				delete mTransactionBody;
			}
			mTransactionBody = nullptr;
			mProtoGradidoTransaction = nullptr;
		}

		bool GradidoTransaction::validate(
			TransactionValidationLevel level/* = TRANSACTION_VALIDATION_SINGLE*/,
			IGradidoBlockchain* blockchain/* = nullptr*/,
			const ConfirmedTransaction* parentConfirmedTransaction/* = nullptr*/,
			IGradidoBlockchain* otherBlockchain/* = nullptr*/
		) const
		{
			if ((level & TRANSACTION_VALIDATION_SINGLE) == TRANSACTION_VALIDATION_SINGLE)
			{
				auto sig_map = mProtoGradidoTransaction->sig_map();

				// check if all signatures belong to current body bytes
				auto body_bytes = mProtoGradidoTransaction->body_bytes();
				for (auto it = sig_map.sig_pair().begin(); it != sig_map.sig_pair().end(); it++)
				{
					if (it->pubkey().size() != KeyPairEd25519::getPublicKeySize()) {
						throw TransactionValidationInvalidInputException(
							"pubkey hasn't the correct size", "pubkey", "binary string 32"
						);
					}
					if (it->signature().size() != 64) {
						throw TransactionValidationInvalidInputException(
							"signature hasn't correct size", "signature", "binary string 64"
						);
					}
					KeyPairEd25519 key_pair((const unsigned char*)it->pubkey().data());
					if (!key_pair.verify(body_bytes, it->signature())) {
						throw TransactionValidationInvalidSignatureException(
							"pubkey don't belong to body bytes", it->pubkey(), it->signature(), body_bytes
						);
					}
				}

				// check for not allowed signatures
				mTransactionBody->getTransactionBase()->checkRequiredSignatures(&sig_map);
				mTransactionBody->validate(level, blockchain, parentConfirmedTransaction);
			}

			// must be implemented in gradido node server
			if ((level & TRANSACTION_VALIDATION_PAIRED) == TRANSACTION_VALIDATION_PAIRED)
			{
				assert(otherBlockchain);

				auto transactionBody = getTransactionBody();
				auto mm = MemoryManager::getInstance();
				Poco::SharedPtr<TransactionEntry> pairTransactionEntry;
				switch (transactionBody->getCrossGroupType()) {
				case proto::gradido::TransactionBody_CrossGroupType_LOCAL: break; // no cross grou�
				case proto::gradido::TransactionBody_CrossGroupType_INBOUND:
					// happen before inbound, can only be checked after both transactions are written to blockchain
				case proto::gradido::TransactionBody_CrossGroupType_OUTBOUND:
				case proto::gradido::TransactionBody_CrossGroupType_CROSS:
					if (!mProtoGradidoTransaction->parent_message_id().size()) {
						throw TransactionValidationInvalidInputException("parent message id not set for outbound", "parent message id", "binary");
					}
					else {
						auto parentMessageId = getParentMessageId();
						pairTransactionEntry = otherBlockchain->findByMessageId(parentMessageId, false);
						mm->releaseMemory(parentMessageId);
						if (pairTransactionEntry.isNull()) {
							throw TransactionValidationException("pairing transaction not found");
						}
						auto pairingConfirmedTransaction = std::make_unique<ConfirmedTransaction>(pairTransactionEntry->getSerializedTransaction());
						auto pairingTransaction = pairingConfirmedTransaction->getGradidoTransaction();
						if (!isBelongToUs(pairingTransaction)) {
							throw PairingTransactionNotMatchException(
								"pairing transaction not matching",
								getSerializedConst().get(),
								pairingTransaction->getSerializedConst().get()
							);
						}
					}
					break;
				default: throw GradidoUnknownEnumException("unknown cross group type", "proto::gradido::TransactionBody_CrossGroupType", transactionBody->getCrossGroupType());
				}

			}

			return true;
		}

		bool GradidoTransaction::isBelongToUs(const GradidoTransaction* pairingTransaction) const
		{
			auto publicKeys = getPublicKeysfromSignatureMap(false);
			auto publicKeysPair = pairingTransaction->getPublicKeysfromSignatureMap(false);
			bool result = true;
			auto mm = MemoryManager::getInstance();

			if (publicKeys.size() != publicKeysPair.size()) {
				result = false;
			}
			else {
				for (int i = 0; i < publicKeys.size(); i++) {
					if (!publicKeys[i]->isSame(publicKeysPair[i])) {
						result = false;
					}
					mm->releaseMemory(publicKeys[i]);
					mm->releaseMemory(publicKeysPair[i]);
				}
			}
			if (!result) return false;
			return getTransactionBody()->isBelongToUs(pairingTransaction->getTransactionBody());
		}

		bool GradidoTransaction::addSign(const MemoryBin* pubkeyBin, const MemoryBin* signatureBin)
		{
			std::unique_ptr<std::string> bodyBytes;

			bodyBytes = mTransactionBody->getBodyBytes();
			auto sigMap = mProtoGradidoTransaction->mutable_sig_map();

			// check if pubkey already exist
			for (auto it = sigMap->sig_pair().begin(); it != sigMap->sig_pair().end(); it++)
			{
				if (it->pubkey().size() != KeyPairEd25519::getPublicKeySize()) {
					throw TransactionValidationInvalidInputException("invalid size", "public key");
				}
				if (0 == memcmp(pubkeyBin, it->pubkey().data(), KeyPairEd25519::getPublicKeySize())) {
					throw TransactionValidationInvalidInputException("already exist", "public key");
				}
			}

			auto sigPair = sigMap->add_sig_pair();
			sigPair->mutable_pubkey()->assign((const char*)pubkeyBin->data(), crypto_sign_PUBLICKEYBYTES);
			sigPair->mutable_signature()->assign((const char*)signatureBin->data(), crypto_sign_BYTES);
			return sigMap->sig_pair_size() >= mTransactionBody->getTransactionBase()->getMinSignatureCount();
		}

		int GradidoTransaction::getSignCount() const
		{
			return mProtoGradidoTransaction->sig_map().sig_pair_size();
		}

		std::vector<std::pair<MemoryBin*, MemoryBin*>> GradidoTransaction::getPublicKeySignaturePairs(bool withPublicKey, bool withSignatures, bool onlyFirst/* = true*/) const
		{
			auto mm = MemoryManager::getInstance();
			assert(withPublicKey || withSignatures);
			std::vector<std::pair<MemoryBin*, MemoryBin*>> result;
			if (!onlyFirst) {
				result.reserve(getSignCount());
			}
			auto sigMap = mProtoGradidoTransaction->sig_map();

			MemoryBin* pubkey = nullptr;
			MemoryBin* signature = nullptr;

			for (int i = 0; i < sigMap.sig_pair_size(); i++) {
				auto sigPair = sigMap.sig_pair()[i];
				if (withPublicKey) {
					pubkey = mm->getMemory(sigPair.pubkey().size());
					pubkey->copyFromProtoBytes(sigPair.pubkey());
				}

				if (withSignatures) {
					signature = mm->getMemory(sigPair.signature().size());
					signature->copyFromProtoBytes(sigPair.signature());
				}
				result.push_back({ pubkey, signature });
				if (onlyFirst) break;
			}
			return result;
		}

		std::vector<MemoryBin*> GradidoTransaction::getPublicKeysfromSignatureMap(bool onlyFirst/* = true*/) const
		{
			auto publicSignatures = getPublicKeySignaturePairs(true, false, onlyFirst);

			std::vector<MemoryBin*> result;
			if (!onlyFirst) {
				result.reserve(publicSignatures.size());
			}
			for (int i = 0; i < publicSignatures.size(); i++) {
				result.push_back(publicSignatures[i].first);
				if (onlyFirst) break;
			}

			return result;
		}

		std::vector<MemoryBin*> GradidoTransaction::getSignaturesfromSignatureMap(bool onlyFirst/* = true*/) const
		{
			auto publicSignatures = getPublicKeySignaturePairs(false, true, onlyFirst);

			std::vector<MemoryBin*> result;
			if (!onlyFirst) {
				result.reserve(publicSignatures.size());
			}
			for (int i = 0; i < publicSignatures.size(); i++) {
				result.push_back(publicSignatures[i].second);
				if (onlyFirst) break;
			}

			return result;
		}

		GradidoTransaction& GradidoTransaction::setParentMessageId(const MemoryBin* parentMessageId)
		{
			mProtoGradidoTransaction->set_allocated_parent_message_id(
				parentMessageId->copyAsString().release()
			);
			return *this;
		}

		void GradidoTransaction::updateBodyBytes()
		{
			if (mBodyDirty) {
				if (!isBodyLocked()) {
					mProtoGradidoTransaction->set_allocated_body_bytes(mTransactionBody->getBodyBytes().release());
					mBodyDirty = false;
				}
				else {
					// why? The protobuf transaction from node js gradido are in a very old proto format, they can be read but the signatures don't match
					// protobuf seems to be backwards compatible 
					throw TransactionBodyLockedException("try to updateBodyBytes but body is locked, at least one signature exist!");
				}
			}
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
			updateBodyBytes();
			auto size = mProtoGradidoTransaction->ByteSizeLong();
			//auto bodyBytesSize = MemoryManager::getInstance()->getFreeMemory(mProtoCreation.ByteSizeLong());
			std::string* resultString(new std::string(size, 0));
			if (!mProtoGradidoTransaction->SerializeToString(resultString)) {
				//addError(new Error("TransactionCreation::getBodyBytes", "error serializing string"));
				throw ProtobufSerializationException(mProtoGradidoTransaction);
			}
			std::unique_ptr<std::string> result;
			result.reset(resultString);
			return result;
		}

		std::unique_ptr<std::string> GradidoTransaction::getSerializedConst() const
		{
			auto size = mProtoGradidoTransaction->ByteSizeLong();
			//auto bodyBytesSize = MemoryManager::getInstance()->getFreeMemory(mProtoCreation.ByteSizeLong());
			std::string* resultString(new std::string(size, 0));
			if (!mProtoGradidoTransaction->SerializeToString(resultString)) {
				//addError(new Error("TransactionCreation::getBodyBytes", "error serializing string"));
				throw ProtobufSerializationException(mProtoGradidoTransaction);
			}
			std::unique_ptr<std::string> result;
			result.reset(resultString);
			return result;
		}

		std::string GradidoTransaction::toJson(bool replaceBase64WithHex/* = true*/) const
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
			status = google::protobuf::util::MessageToJsonString(*mProtoGradidoTransaction, &json_message, options);
			if (!status.ok()) {
				throw ProtobufJsonSerializationException("error parsing transaction", *mProtoGradidoTransaction, status);
			}
			//\"bodyBytes\": \"MigKIC7Sihz14RbYNhVAa8V3FSIhwvd0pWVvZqDnVA91dtcbIgRnZGQx\"
			int startBodyBytes = json_message.find("bodyBytes") + std::string("\"bodyBytes\": \"").size() - 2;
			int endCur = json_message.find_first_of('\"', startBodyBytes + 2) + 1;
			json_message.replace(startBodyBytes, endCur - startBodyBytes, json_message_body);
			//printf("json: %s\n", json_message.data());

			if (replaceBase64WithHex) {
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
			}

			return json_message;
		}

		bool GradidoTransaction::isBodyLocked()
		{
			return getSignCount() > 0;
		}
	}
}
