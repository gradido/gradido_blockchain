#include "gradido_blockchain/model/protobufWrapper/GradidoBlock.h"
#include <google/protobuf/util/json_util.h>
#include "gradido/TransactionBody.pb.h"

#include "gradido_blockchain/lib/DataTypeConverter.h"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"

#include "gradido_blockchain/model/protobufWrapper/ProtobufExceptions.h"

using namespace rapidjson;

namespace model {
	namespace gradido {
		GradidoBlock::GradidoBlock(const std::string& serializedGradidoBlock)
			: mGradidoTransaction(nullptr)
		{
			if (!mProtoGradidoBlock.ParseFromString(serializedGradidoBlock)) {
				throw ProtobufParseException(serializedGradidoBlock);
			}
			mGradidoTransaction = new GradidoTransaction(mProtoGradidoBlock.mutable_transaction());
		}

		GradidoBlock::~GradidoBlock()
		{
			if (mGradidoTransaction) {
				delete mGradidoTransaction;
			}
		}

		std::string GradidoBlock::toJson()
		{
			std::string json_message = "";
			std::string json_message_body = "";
			google::protobuf::util::JsonPrintOptions options;
			options.add_whitespace = true;
			options.always_print_primitive_fields = true;

			auto status = google::protobuf::util::MessageToJsonString(mProtoGradidoBlock, &json_message, options);
			if (!status.ok()) {
				throw ProtobufJsonSerializationException("error parsing gradido block", mProtoGradidoBlock, status);
			}
			proto::gradido::TransactionBody body;
			body.ParseFromString(mProtoGradidoBlock.transaction().body_bytes());
			status = google::protobuf::util::MessageToJsonString(body, &json_message_body, options);
			if (!status.ok()) {				
				throw ProtobufJsonSerializationException("error parsing transaction body", mProtoGradidoBlock, status);
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
			mProtoGradidoBlock.mutable_transaction()->set_allocated_body_bytes(mGradidoTransaction->getTransactionBody()->getBodyBytes().release());
			//mProtoGradidoTransaction->set_allocated_body_bytes(mTransactionBody->getBodyBytes().release());

			auto size = mProtoGradidoBlock.ByteSizeLong();
			//auto bodyBytesSize = MemoryManager::getInstance()->getFreeMemory(mProtoCreation.ByteSizeLong());
			std::string* resultString(new std::string(size, 0));
			if (!mProtoGradidoBlock.SerializeToString(resultString)) {
				//addError(new Error("TransactionCreation::getBodyBytes", "error serializing string"));
				throw ProtobufSerializationException(mProtoGradidoBlock);
			}
			std::unique_ptr<std::string> result;
			result.reset(resultString);
			return result;
		}

	}
}