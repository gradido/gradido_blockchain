#ifndef __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_GRADIDO_TRANSACTION_H
#define __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_GRADIDO_TRANSACTION_H

#include "TransactionBody.h"
#include "proto/gradido/gradido_transaction.pb.h"
#include "gradido_blockchain/model/protopufTypes.h"

class TransactionFactory;
class CrossGroupTransactionBuilder;

namespace model {

	namespace gradido {

		class ConfirmedTransaction;

		/*!
		 *  \addtogroup Gradido-Protocol
 		 *  @{
 		 */

		//!  Contain the signature map for the Transaction and the serialized body
		/*!

			<a href="https://github.com/gradido/gradido_protocol/blob/master/gradido/GradidoTransaction.proto" target="_blank">Protobuf: GradidoTransaction</a>
		*/
		class GRADIDOBLOCKCHAIN_EXPORT GradidoTransaction
		{
			friend ConfirmedTransaction;
		public:
			GradidoTransaction(proto::gradido::GradidoTransaction* protoGradidoTransaction, std::shared_ptr<ProtobufArenaMemory> arenaMemory);
			GradidoTransaction(const std::string* serializedProtobuf);
			GradidoTransaction(const void* data, int size);
			GradidoTransaction(model::gradido::TransactionBody* body);
			~GradidoTransaction();

			inline const TransactionBody* getTransactionBody() const { return mTransactionBody; }
			inline TransactionBody* getMutableTransactionBody() { mBodyDirty = true; return mTransactionBody; }

			/*! Check if Transaction is valid, calls validate from TransactionBody
				Details
			*/
			bool validate(
				TransactionValidationLevel level = TRANSACTION_VALIDATION_SINGLE,
				IGradidoBlockchain* blockchain = nullptr,
				const ConfirmedTransaction* parentGradidoBlock = nullptr,
				IGradidoBlockchain* otherBlockchain = nullptr
			) const;

			bool isBelongToUs(const GradidoTransaction* pairingTransaction) const;

			bool addSign(const MemoryBin* pubkeyBin, const MemoryBin* signatureBin);

			int getSignCount() const;
			// caller must release MemoryBin afterwards
			inline std::vector<std::pair<MemoryBin*, MemoryBin*>> getPublicKeySignaturePairs(bool onlyFirst = true) const { return getPublicKeySignaturePairs(true, true, onlyFirst); }
			std::vector<MemoryBin*> getPublicKeysfromSignatureMap(bool onlyFirst = true) const;
			std::vector<MemoryBin*> getSignaturesfromSignatureMap(bool onlyFirst = true) const;

			inline GradidoTransaction& setMemo(const std::string& memo) { mBodyDirty = true; mTransactionBody->setMemo(memo); return *this; }
			inline GradidoTransaction& setCreatedAt(Timepoint createdAt) { mBodyDirty = true; mTransactionBody->setCreatedAt(createdAt); return *this; }
			GradidoTransaction& setParentMessageId(const MemoryBin* parentMessageId);
			/*! update body bytes into proto member
				Serialize body bytes from TransactionBody member Variable and stuff it into proto::gradido::GradidoTransaction bodyBytes
			*/
			void updateBodyBytes();

			/*! deliver message id from cross group transaction pair (outbound) or nullptr
				@return MemoryBin containing message id binar, must be freed from caller
			*/
			MemoryBin* getParentMessageId() const;

			std::unique_ptr<std::string> getSerialized();
			std::unique_ptr<std::string> getSerializedConst() const;
			std::string toJson(bool replaceBase64WithHex = true) const;

			inline std::shared_ptr<ProtobufArenaMemory> getProtobufArena() { return mProtobufArenaMemory; }

		protected:
			std::vector<std::pair<MemoryBin*, MemoryBin*>> getPublicKeySignaturePairs(bool withPublicKey, bool withSignatures, bool onlyFirst = true) const;
			inline proto::gradido::GradidoTransaction* getProto() { return mProtoGradidoTransaction; }
			//! \return true if at least one signature exist on GradidoTransaction, updating body bytes would invalidate the signature
			bool isBodyLocked();
			proto::gradido::GradidoTransaction* mProtoGradidoTransaction;
			TransactionBody* mTransactionBody;
			std::shared_ptr<ProtobufArenaMemory> mProtobufArenaMemory;
			//! set to true if body was changed 
			bool mBodyDirty;
		};
		/*! @} End of Doxygen Groups*/

		class TransactionBodyLockedException : public GradidoBlockchainException
		{
		public:
			explicit TransactionBodyLockedException(const char* what) : GradidoBlockchainException(what) {}
			~TransactionBodyLockedException() {};
			std::string getFullString() const { return what();  }
		protected:
		};
	}
}



#endif // __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_GRADIDO_TRANSACTION_H
