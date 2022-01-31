#include "GradidoTransaction.h"

#include "TransactionValidationExceptions.h"

#include "Crypto/KeyPairEd25519.h"
#include "ProtobufExceptions.h"

namespace model {
	namespace gradido {
		GradidoTransaction::GradidoTransaction(proto::gradido::GradidoTransaction* protoGradidoTransaction)
			: mProtoGradidoTransaction(protoGradidoTransaction), mTransactionBody(nullptr)
		{
			mTransactionBody = TransactionBody::load(protoGradidoTransaction->body_bytes());
		}

		GradidoTransaction::GradidoTransaction(const std::string& serializedProtobuf)
		{
			mProtoGradidoTransaction = new proto::gradido::GradidoTransaction;
			if (!mProtoGradidoTransaction->ParseFromString(serializedProtobuf)) {
				throw ProtobufParseException(serializedProtobuf);
			}
			mTransactionBody = TransactionBody::load(mProtoGradidoTransaction->body_bytes());
		}

		GradidoTransaction::GradidoTransaction(model::gradido::TransactionBody* body)
		{
			assert(body);
			mProtoGradidoTransaction = new proto::gradido::GradidoTransaction;
			mTransactionBody = body;
		}

		GradidoTransaction::~GradidoTransaction()
		{
			if (mTransactionBody) {
				delete mTransactionBody;
			}
		}

		bool GradidoTransaction::validate(
			TransactionValidationLevel level/* = TRANSACTION_VALIDATION_SINGLE*/,
			IGradidoBlockchain* blockchain/* = nullptr*/,
			IGradidoBlockchain* otherBlockchain/* = nullptr*/
		) const
		{
			if ((level & TRANSACTION_VALIDATION_SINGLE) == TRANSACTION_VALIDATION_SINGLE) 
			{
				auto sig_map = mProtoGradidoTransaction->sig_map();

				// check if all signatures belong to current body bytes
				auto body_bytes = mProtoGradidoTransaction->body_bytes();
				for (auto it = sig_map.sigpair().begin(); it != sig_map.sigpair().end(); it++)
				{
					KeyPairEd25519 key_pair((const unsigned char*)it->pubkey().data());
					if (!key_pair.verify(body_bytes, it->signature())) {
						throw TransactionValidationInvalidSignatureException(
							"pubkey don't belong to bodybytes", it->pubkey(), it->signature(), body_bytes
						);
					}
				}

				auto transaction_base = mTransactionBody->getTransactionBase();
				auto result = transaction_base->checkRequiredSignatures(&sig_map);

				transaction_base->validate();
			}
			/* 
			// must be implemented in gradido node server
			if ((level & TRANSACTION_VALIDATION_PAIRED) == TRANSACTION_VALIDATION_PAIRED)
			{
				auto transactionBody = getTransactionBody();
				if (transactionBody->getCrossGroupType() == proto::gradido::TransactionBody_CrossGroupType_LOCAL) {
					return true;
				}
				if (transactionBody->getCrossGroupType() == proto::gradido::TransactionBody_CrossGroupType_INBOUND) {

				}
			}
			*/
			return true;

		}

		std::unique_ptr<std::string> GradidoTransaction::getSerialized()
		{
			mProtoGradidoTransaction->set_allocated_body_bytes(mTransactionBody->getBodyBytes().release());

			auto size = mProtoGradidoTransaction->ByteSizeLong();
			//auto bodyBytesSize = MemoryManager::getInstance()->getFreeMemory(mProtoCreation.ByteSizeLong());
			std::string* resultString(new std::string(size, 0));
			if (!mProtoGradidoTransaction->SerializeToString(resultString)) {
				//addError(new Error("TransactionCreation::getBodyBytes", "error serializing string"));
				throw ProtobufSerializationException(*mProtoGradidoTransaction);
			}
			std::unique_ptr<std::string> result;
			result.reset(resultString);
			return result;
		}
	}
}