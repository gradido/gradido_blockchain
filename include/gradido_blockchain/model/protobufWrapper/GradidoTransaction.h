#ifndef __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_GRADIDO_TRANSACTION_H
#define __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_GRADIDO_TRANSACTION_H

#include "TransactionBody.h"
#include "proto/gradido/GradidoTransaction.pb.h"
#include "Poco/RefCountedObject.h"

class TransactionFactory;
class CrossGroupTransactionBuilder;

namespace model {	

	namespace gradido {

		class GradidoBlock;

		/*!
		 *  \addtogroup Gradido-Protocol
 		 *  @{
 		 */

		//!  Contain the signature map for the Transaction and the serialized body
		/*!
	  		
			<a href="https://github.com/gradido/gradido_protocol/blob/master/gradido/GradidoTransaction.proto" target="_blank">Protobuf: GradidoTransaction</a>
		*/
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
			
			//! Check if Transaction is valid, calls validate from TransactionBody
			/*!
				Details
			*/
			bool validate(
				TransactionValidationLevel level = TRANSACTION_VALIDATION_SINGLE,
				IGradidoBlockchain* blockchain = nullptr,
				const GradidoBlock* parentGradidoBlock = nullptr,
				IGradidoBlockchain* otherBlockchain = nullptr
			) const;

			bool isBelongToUs(const GradidoTransaction* pairingTransaction) const;

			bool addSign(const MemoryBin* pubkeyBin, const MemoryBin* signatureBin);

			int getSignCount() const { return mProtoGradidoTransaction->sig_map().sigpair_size(); }
			// caller must release MemoryBin afterwards
			inline std::vector<std::pair<MemoryBin*, MemoryBin*>> getPublicKeySignaturePairs(bool onlyFirst = true) const { return getPublicKeySignaturePairs(true, true, onlyFirst); }
			std::vector<MemoryBin*> getPublicKeysfromSignatureMap(bool onlyFirst = true) const;
			std::vector<MemoryBin*> getSignaturesfromSignatureMap(bool onlyFirst = true) const;

			inline GradidoTransaction& setMemo(const std::string& memo) { mTransactionBody->setMemo(memo); return *this; }
			inline GradidoTransaction& setCreated(Poco::DateTime created) { mTransactionBody->setCreated(created); return *this; }
			inline GradidoTransaction& setParentMessageId(const MemoryBin* parentMessageId) { mProtoGradidoTransaction->set_allocated_parent_message_id(parentMessageId->copyAsString().release()); return *this; }
			inline GradidoTransaction& setApolloTransactionId(uint64_t apolloTransactionId) { mProtoGradidoTransaction->set_apollo_transaction_id(apolloTransactionId); return *this; }
			void updateBodyBytes();

			//! \return MemoryBin containing message id binar, must be freed from caller
			MemoryBin* getParentMessageId() const;
			inline uint64_t getApolloTransactionId() { return mProtoGradidoTransaction->apollo_transaction_id(); }

			std::unique_ptr<std::string> getSerialized();
			std::unique_ptr<std::string> getSerializedConst() const;
			std::string toJson() const;

		protected:
			std::vector<std::pair<MemoryBin*, MemoryBin*>> getPublicKeySignaturePairs(bool withPublicKey, bool withSignatures, bool onlyFirst = true) const;
			inline proto::gradido::GradidoTransaction* getProto() { return mProtoGradidoTransaction; }
			proto::gradido::GradidoTransaction* mProtoGradidoTransaction;
			TransactionBody* mTransactionBody;
		};
	}
}



#endif // __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_GRADIDO_TRANSACTION_H