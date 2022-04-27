#ifndef __GRADIDO_LOGIN_SERVER_MODEL_GRADIDO_GRADIDO_BLOCK
#define __GRADIDO_LOGIN_SERVER_MODEL_GRADIDO_GRADIDO_BLOCK

#include "proto/gradido/GradidoBlock.pb.h"
#include "GradidoTransaction.h"

#include "Poco/SharedPtr.h"

#define GRADIDO_BLOCK_PROTOCOL_VERSION 3
// MAGIC NUMBER: max allowed Timespan between creation date of transaction and receiving date (iota milestone timestamp) 
// taken 2 minutes from hedera but maybe the time isn't enough if gradido is more used
#define MAGIC_NUMBER_MAX_TIMESPAN_BETWEEN_CREATING_AND_RECEIVING_TRANSACTION_IN_MINUTES 2

namespace model {
	namespace gradido {
		/*!
		 *  \addtogroup Gradido-Protocol
 		 *  @{
 		 */

		//!  The Final Blockformat on Blockchain
		/*!
	  		The Format in which Gradido Transaction are saved on Blockchain from Gradido Node,
		    also the native format in which Gradido Node returns Gradido Transactions.<br>
			<a href="https://github.com/gradido/gradido_protocol/blob/master/gradido/GradidoBlock.proto" target="_blank">Protobuf: GradidoBlock</a>
		*/
		class GRADIDOBLOCKCHAIN_EXPORT GradidoBlock : public MultithreadContainer, public Poco::RefCountedObject
		{
		public:
			GradidoBlock(const std::string* serializedGradidoBlock);
			~GradidoBlock();

			static Poco::SharedPtr<GradidoBlock> create(std::unique_ptr<GradidoTransaction> transaction, uint64_t id, int64_t received, const MemoryBin* messageId);

			//! make json string from GradidoBlock Protobuf Object
			std::string toJson();
			rapidjson::Value toJson(rapidjson::Document& baseDocument);
			inline const GradidoTransaction* getGradidoTransaction() const { return mGradidoTransaction; }

			//! Check if Transaction is valid, calls validate from GradidoTransaction
			/*! Throw a exception derived from TransactionValidationException if some validation rule isn't met.<br><br>
				<b>level = TRANSACTION_VALIDATION_SINGLE</b>
				<ul>
					<li>Check Gradido Block Protocol Version</li>
					<li>Check if (iota) Message ID exist</li>
					<li>Check that not more than 2 minutes has passed after created date from TransactionBody until received date (iota Milestone Timestamp)</li>
					<li>Check that received date is older than created date from TransactionBody
				</ul>
				<b>level = TRANSACTION_VALIDATION_SINGLE_PREVIOUS, blockchain != nullptr</b>
				<ul>
					<li>Check that the transaction with id-1 exist</li>
					<li>Check that received date from previous transaction is younger</li>
					<li>Call calculateTxHash() with previous transaction and check that it is equal to stored txHash (runningHash)</li> 
				</ul>
				Call GradidoTransaction::validate()
			*/
			bool validate(
				TransactionValidationLevel level = TRANSACTION_VALIDATION_SINGLE,
				IGradidoBlockchain* blockchain = nullptr,
				IGradidoBlockchain* otherBlockchain = nullptr
			) const;
			
			// proto member variable accessors
			inline uint64_t getID() const { return mProtoGradidoBlock->id(); }
			inline const std::string& getTxHash() const { return mProtoGradidoBlock->running_hash(); }
			inline const std::string& getFinalBalance() const { return mProtoGradidoBlock->final_gdd(); }

			inline void setTxHash(const MemoryBin* txHash) { mProtoGradidoBlock->set_allocated_running_hash(txHash->copyAsString().release()); }
			// convert from proto timestamp seconds to poco DateTime
			inline Poco::Timestamp getReceivedAsTimestamp() const {
				return Poco::Timestamp(mProtoGradidoBlock->received().seconds() * Poco::Timestamp::resolution());
			}
			// return as seconds since Jan 01 1970
			inline int64_t getReceived() const { return mProtoGradidoBlock->received().seconds(); }

			std::unique_ptr<std::string> getSerialized();
			//! \return MemoryBin containing message id binar, must be freed from caller
			MemoryBin* getMessageId() const;
			std::string getMessageIdHex() const;

			//! calculate tx hash for this Gradido Block using last txHash as start
			//! \return caller must free result
			MemoryBin* calculateTxHash(const GradidoBlock* previousBlock) const;

			//! \brief calculate final gdd for this transaction and set value into proto structure
			//! 
			//! for transfer transaction, it is the final balance from sender
			void calculateFinalGDD(IGradidoBlockchain* blockchain);

		protected:
			// TODO: use Pool for reducing memory allocation for google protobuf objects
			proto::gradido::GradidoBlock* mProtoGradidoBlock;
			GradidoTransaction* mGradidoTransaction;
			GradidoBlock(std::unique_ptr<GradidoTransaction> transaction);
		};

		/*! @} End of Doxygen Groups*/
	}
}

#endif //__GRADIDO_LOGIN_SERVER_MODEL_GRADIDO_GRADIDO_BLOCK