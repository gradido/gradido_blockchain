#ifndef __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_GRADIDO_TRANSACTION_H
#define __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_GRADIDO_TRANSACTION_H

#include "TransactionBody.h"
#include "gradido/GradidoTransaction.pb.h"
#include "Poco/RefCountedObject.h"

class TransactionFactory;
class CrossGroupTransactionBuilder;

namespace model {	

	namespace gradido {

		class GradidoBlock;

		class GRADIDOBLOCKCHAIN_EXPORT GradidoTransaction: public Poco::RefCountedObject
		{
			friend GradidoBlock;
		public:
			GradidoTransaction(proto::gradido::GradidoTransaction* protoGradidoTransaction);
			GradidoTransaction(const std::string* serializedProtobuf);
			GradidoTransaction(model::gradido::TransactionBody* body);
			~GradidoTransaction();

			inline const TransactionBody* getTransactionBody() const { return mTransactionBody; }
			inline TransactionBody* getMutableTransactionBody() { return mTransactionBody; }
			inline const proto::gradido::GradidoTransaction* getProto() const { return mProtoGradidoTransaction; }
			
			bool validate(
				TransactionValidationLevel level = TRANSACTION_VALIDATION_SINGLE,
				IGradidoBlockchain* blockchain = nullptr,
				const GradidoBlock* parentGradidoBlock = nullptr,
				IGradidoBlockchain* otherBlockchain = nullptr
			) const;

			bool isBelongToUs(const GradidoTransaction* pairingTransaction) const;

			bool addSign(const MemoryBin* pubkeyBin, const MemoryBin* signatureBin);

			int getSignCount() const { return mProtoGradidoTransaction->sig_map().sigpair_size(); }
 			const proto::gradido::SignatureMap&	getSigMap() const { return mProtoGradidoTransaction->sig_map(); }

			inline GradidoTransaction& setMemo(const std::string& memo) { mTransactionBody->setMemo(memo); return *this; }
			inline GradidoTransaction& setCreated(Poco::DateTime created) { mTransactionBody->setCreated(created); return *this; }
			inline GradidoTransaction& setParentMessageId(MemoryBin* parentMessageId) { mProtoGradidoTransaction->set_allocated_parent_message_id(parentMessageId->copyAsString().release()); return *this; }

			//! \return MemoryBin containing message id binar, must be freed from caller
			MemoryBin* getParentMessageId() const;

			std::unique_ptr<std::string> getSerialized();
			std::unique_ptr<std::string> getSerializedConst() const;
			std::string toJson() const;

		protected:
			inline proto::gradido::GradidoTransaction* getProto() { return mProtoGradidoTransaction; }
			proto::gradido::GradidoTransaction* mProtoGradidoTransaction;
			TransactionBody* mTransactionBody;
		};
	}
}



#endif // __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_GRADIDO_TRANSACTION_H