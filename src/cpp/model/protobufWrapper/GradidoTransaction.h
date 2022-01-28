#ifndef __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_GRADIDO_TRANSACTION_H
#define __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_GRADIDO_TRANSACTION_H

#include "TransactionBody.h"
#include "gradido/GradidoTransaction.pb.h"

namespace model {
	namespace gradido {

		class GradidoTransaction
		{
		public:
			GradidoTransaction();
			~GradidoTransaction();

			inline const std::shared_ptr<TransactionBody> getTransactionBody() const { return mTransactionBody; }
			bool validate(
				TransactionValidationLevel level = TRANSACTION_VALIDATION_SINGLE,
				IGradidoBlockchain* blockchain = nullptr,
				IGradidoBlockchain* otherBlockchain = nullptr
			) const;

		protected:
			proto::gradido::GradidoTransaction* mProtoGradidoTransaction;
			std::shared_ptr<TransactionBody> mTransactionBody;

		};
	}
}



#endif // __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_GRADIDO_TRANSACTION_H