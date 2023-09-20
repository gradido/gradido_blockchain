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
#include "proto/gradido/gradido_transfer.pb.h"


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
				const ConfirmedTransaction* parentConfirmedTransaction = nullptr
			) const;

			std::vector<MemoryBin*> getInvolvedAddresses() const;
			bool isInvolved(const std::string pubkeyString) const;
			const std::string& getCoinCommunityId() const;
			[[deprecated("Replaced by getCoinCommunityId, changed name according to Gradido Apollo implementation")]]
			inline const std::string& getCoinGroupId() const { return getCoinCommunityId(); }
			bool isBelongToUs(const TransactionBase* pairingTransaction) const;

			const std::string& getAmount() const;
			const std::string& getSenderPublicKeyString() const;
			const std::string& getRecipientPublicKeyString() const;
			MemoryBin* getSenderPublicKey() const;
			MemoryBin* getRecipientPublicKey() const;

			std::string toDebugString() const;

		protected:
			const proto::gradido::GradidoTransfer& mProtoTransfer;
		};
	}
}


#endif //GRADIDO_LOGIN_SERVER_MODEL_TRANSACTION_TRANSFER_INCLUDE
