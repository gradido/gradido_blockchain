#ifndef __GRADIDO_LOGIN_SERVER_MODEL_GRADIDO_GRADIDO_BLOCK
#define __GRADIDO_LOGIN_SERVER_MODEL_GRADIDO_GRADIDO_BLOCK

#include "gradido/GradidoBlock.pb.h"
#include "GradidoTransaction.h"

#include "Poco/SharedPtr.h"

#define GRADIDO_BLOCK_PROTOCOL_VERSION 3

namespace model {
	namespace gradido {
		class GRADIDOBLOCKCHAIN_EXPORT GradidoBlock : public MultithreadContainer, public Poco::RefCountedObject
		{
		public:
			GradidoBlock(std::unique_ptr<std::string> serializedGradidoBlock);
			GradidoBlock(const std::string* serializedGradidoBlock);
			~GradidoBlock();

			static Poco::SharedPtr<GradidoBlock> create(std::unique_ptr<GradidoTransaction> transaction, uint64_t id, int64_t received, const MemoryBin* messageId);

			std::string toJson();
			inline const GradidoTransaction* getGradidoTransaction() const { return mGradidoTransaction; }

			bool validate(
				TransactionValidationLevel level = TRANSACTION_VALIDATION_SINGLE,
				IGradidoBlockchain* blockchain = nullptr,
				IGradidoBlockchain* otherBlockchain = nullptr
			) const;
			
			// proto member variable accessors
			inline uint64_t getID() const { return mProtoGradidoBlock.id(); }
			inline const std::string& getTxHash() const { return mProtoGradidoBlock.running_hash(); }
			inline void setTxHash(const MemoryBin* txHash) { mProtoGradidoBlock.set_allocated_running_hash(txHash->copyAsString().release()); }
			// convert from proto timestamp seconds to poco DateTime
			inline Poco::DateTime getReceived() const {
				return Poco::Timestamp(mProtoGradidoBlock.received().seconds() * Poco::Timestamp::resolution());
			}

			std::unique_ptr<std::string> getSerialized();

			//! \return caller must free result
			MemoryBin* calculateTxHash(std::shared_ptr<GradidoBlock> previousBlock) const;
			void calculateFinalGDD(const IGradidoBlockchain* blockchain);

		protected:
			proto::gradido::GradidoBlock mProtoGradidoBlock;
			GradidoTransaction* mGradidoTransaction;
			GradidoBlock(std::unique_ptr<GradidoTransaction> transaction);
		};
	}
}

#endif //__GRADIDO_LOGIN_SERVER_MODEL_GRADIDO_GRADIDO_BLOCK