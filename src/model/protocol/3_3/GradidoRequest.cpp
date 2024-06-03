#include "gradido_blockchain/model/protobufWrapper/GradidoRequest.h"
#include "gradido_blockchain/model/protobufWrapper/ProtobufExceptions.h"

namespace model {
	namespace gradido {
		GradidoRequest::GradidoRequest(const std::string* serializedProtobuf)
		{
			mProtobufArenaMemory = ProtobufArenaMemory::create();
			mProtoGradidoRequest = google::protobuf::Arena::CreateMessage<proto::gradido::GradidoRequest>(*mProtobufArenaMemory);
			if (!mProtoGradidoRequest->ParseFromArray(serializedProtobuf->data(), serializedProtobuf->size())) {
				if (!mProtoGradidoRequest->ParsePartialFromArray(serializedProtobuf->data(), serializedProtobuf->size())) {
					throw ProtobufParseException(*serializedProtobuf);
				}
				else {
					printf("only partial error\n");
					throw ProtobufParseException(*serializedProtobuf);
				}
			}
			//mRequestBody = RequestBody::load(mProtoGradidoRequest->body_bytes(), mProtobufArenaMemory);
			//mProtoGradidoRequest->set_allocated_body_bytes(mRequestBody->getBodyBytes().release());
		}

		GradidoRequest::~GradidoRequest()
		{
			if (mRequestBody) {
				delete mRequestBody;
			}
			mRequestBody = nullptr;
			mProtoGradidoRequest = nullptr;
		}
	}
}

