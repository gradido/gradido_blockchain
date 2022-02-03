#ifndef __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_GRADIDO_TRANSACTION_H
#define __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_GRADIDO_TRANSACTION_H

#include "TransactionBody.h"
#include "gradido/GradidoTransaction.pb.h"

class TransactionFactory;
class CrossGroupTransactionBuilder;

namespace model {	

	namespace gradido {

		class GradidoTransaction
		{
		public:
			GradidoTransaction(proto::gradido::GradidoTransaction* protoGradidoTransaction);
			GradidoTransaction(const std::string& serializedProtobuf);
			GradidoTransaction(model::gradido::TransactionBody* body);
			~GradidoTransaction();

			inline const TransactionBody* getTransactionBody() const { return mTransactionBody; }
			inline TransactionBody* getMutableTransactionBody() { return mTransactionBody; }
			bool validate(
				TransactionValidationLevel level = TRANSACTION_VALIDATION_SINGLE,
				IGradidoBlockchain* blockchain = nullptr,
				IGradidoBlockchain* otherBlockchain = nullptr
			) const;

			inline GradidoTransaction& setMemo(const std::string& memo) { mTransactionBody->setMemo(memo); return *this; }
			inline GradidoTransaction& setCreated(Poco::DateTime created) { mTransactionBody->setCreated(created); return *this; }
			inline GradidoTransaction& setParentMessageId(MemoryBin* parentMessageId) { mProtoGradidoTransaction->set_allocated_parent_message_id(parentMessageId->copyAsString().release()); return *this; }

			std::unique_ptr<std::string> getSerialized();
			std::string toJson();

		protected:
			proto::gradido::GradidoTransaction* mProtoGradidoTransaction;
			TransactionBody* mTransactionBody;

			


		};
	}
}



#endif // __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_GRADIDO_TRANSACTION_H