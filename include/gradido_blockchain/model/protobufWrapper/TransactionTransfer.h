/*!
*
* \author: einhornimmond
*
* \date: 25.10.19
*
* \brief: Creation Transaction
*/
#ifndef GRADIDO_LOGIN_SERVER_MODEL_TRANSACTION_TRANSFER_INCLUDE
#define GRADIDO_LOGIN_SERVER_MODEL_TRANSACTION_TRANSFER_INCLUDE

#pragma warning(disable:4800)

#include "TransactionBase.h"
//#include "Transaction.h"
#include "proto/gradido/GradidoTransfer.pb.h"


namespace model {
	namespace gradido {

		class Transaction;

		class GRADIDOBLOCKCHAIN_EXPORT TransactionTransfer : public TransactionBase
		{
		public:
			TransactionTransfer(const proto::gradido::GradidoTransfer& protoTransfer);
			~TransactionTransfer();

			int prepare();
			bool validate(
				TransactionValidationLevel level = TRANSACTION_VALIDATION_SINGLE,
				IGradidoBlockchain* blockchain = nullptr,
				const GradidoBlock* parentGradidoBlock = nullptr
			) const;

			std::vector<MemoryBin*> getInvolvedAddresses() const;
			uint32_t getCoinColor() const;
			bool isBelongToUs(const TransactionBase* pairingTransaction) const;

			const std::string& getAmount() const { return mProtoTransfer.sender().amount(); }
			MemoryBin* getSenderPublicKey() const;
			MemoryBin* getRecipientPublicKey() const;
			
		protected:
			const proto::gradido::GradidoTransfer& mProtoTransfer;			
		};
	}
}


#endif //GRADIDO_LOGIN_SERVER_MODEL_TRANSACTION_TRANSFER_INCLUDE
