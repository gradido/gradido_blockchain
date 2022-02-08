#ifndef __GRADIDO_LOGIN_SERVER_MODEL_GRADIDO_GRADIDO_BLOCK
#define __GRADIDO_LOGIN_SERVER_MODEL_GRADIDO_GRADIDO_BLOCK

#include "gradido/GradidoBlock.pb.h"
#include "GradidoTransaction.h"

namespace model {
	namespace gradido {
		class GradidoBlock : public MultithreadContainer
		{
		public:
			GradidoBlock(std::string serializedGradidoBlock);
			~GradidoBlock();
			std::string toJson();
			inline const GradidoTransaction* getGradidoTransaction() const { return mGradidoTransaction; }
			

		protected:
			proto::gradido::GradidoBlock mProtoGradidoBlock;
			GradidoTransaction* mGradidoTransaction;

		};
	}
}

#endif //__GRADIDO_LOGIN_SERVER_MODEL_GRADIDO_GRADIDO_BLOCK