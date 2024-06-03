#ifndef __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_GRADIDO_REQUEST_H
#define __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_GRADIDO_REQUEST_H

#include "RequestBody.h"
#include "proto/gradido/gradido_request.pb.h"

namespace model {

	namespace gradido {
		class GRADIDOBLOCKCHAIN_EXPORT GradidoRequest
		{
		public: 
			GradidoRequest(const std::string* serializedProtobuf);
			~GradidoRequest();

		protected:
			std::shared_ptr<ProtobufArenaMemory> mProtobufArenaMemory;
			proto::gradido::GradidoRequest* mProtoGradidoRequest;
			RequestBody* mRequestBody;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_GRADIDO_REQUEST_H