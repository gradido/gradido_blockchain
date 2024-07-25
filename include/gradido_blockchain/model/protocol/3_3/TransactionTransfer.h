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

			std::vector<std::string_view> getInvolvedAddresses() const;
			bool isInvolved(const std::string& pubkeyString) const;
			const std::string& getCoinCommunityId() const;
			bool isBelongToUs(const TransactionBase* pairingTransaction) const;

			const std::string& getAmount() const;
			const std::string& getSenderPublicKey() const;
			const std::string& getRecipientPublicKey() const;

			std::string toDebugString() const;

		protected:
			const proto::gradido::GradidoTransfer& mProtoTransfer;
		};
	}
}


#endif //GRADIDO_LOGIN_SERVER_MODEL_TRANSACTION_TRANSFER_INCLUDE
