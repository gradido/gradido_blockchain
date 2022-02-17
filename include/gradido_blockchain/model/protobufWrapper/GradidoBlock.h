#ifndef __GRADIDO_LOGIN_SERVER_MODEL_GRADIDO_GRADIDO_BLOCK
#define __GRADIDO_LOGIN_SERVER_MODEL_GRADIDO_GRADIDO_BLOCK

#include "gradido/GradidoBlock.pb.h"
#include "GradidoTransaction.h"

namespace model {
	namespace gradido {
		class GradidoBlock : public MultithreadContainer
		{
		public:
			GradidoBlock(const std::string& serializedGradidoBlock);
			~GradidoBlock();
			std::string toJson();
			inline const GradidoTransaction* getGradidoTransaction() const { return mGradidoTransaction; }
			
			// proto member variable accessors
			inline uint64_t getID() { return mProtoGradidoBlock.id(); }
			// convert from proto timestamp seconds to poco DateTime
			inline Poco::DateTime getReceived() const {
				return Poco::Timestamp(mProtoGradidoBlock.received().seconds() * Poco::Timestamp::resolution());
			}

			std::unique_ptr<std::string> getSerialized();

		protected:
			proto::gradido::GradidoBlock mProtoGradidoBlock;
			GradidoTransaction* mGradidoTransaction;

		};
	}
}

#endif //__GRADIDO_LOGIN_SERVER_MODEL_GRADIDO_GRADIDO_BLOCK