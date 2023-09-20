#ifndef __GRADIDO_BLOCKCHAIN_MODEL_PROTBUF_WRAPPER_REQUEST_BODY_H
#define __GRADIDO_BLOCKCHAIN_MODEL_PROTBUF_WRAPPER_REQUEST_BODY_H

#include "proto/gradido/request_body.pb.h"
#include "../../lib/MultithreadContainer.h"

#include "../../MemoryManager.h"

namespace model {
	namespace gradido {
		/*!
		* @author einhornimmond
		* @date 03.12.2022
		* @brief Basic Class for requests (like transactions but not stored in blockchain, requests are more temporary)
		*/
		class GRADIDOBLOCKCHAIN_EXPORT RequestBody : public MultithreadContainer
		{
		public:
			virtual ~RequestBody();

		protected:
			RequestBody();
			RequestBody(std::shared_ptr<ProtobufArenaMemory> arenaMemory);
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_MODEL_PROTBUF_WRAPPER_REQUEST_BODY_H