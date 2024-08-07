#ifndef __GRADIDO_BLOCKCHAIN_PROTOBUF_WRAPPER_DEFERRED_TRANSFER_H
#define __GRADIDO_BLOCKCHAIN_PROTOBUF_WRAPPER_DEFERRED_TRANSFER_H

#include "TransactionTransfer.h"
#include "proto/gradido/gradido_transfer.pb.h"


namespace model {
	namespace gradido {
		class GRADIDOBLOCKCHAIN_EXPORT DeferredTransfer : public TransactionTransfer
		{
		public:
			DeferredTransfer(const proto::gradido::GradidoDeferredTransfer& deferredTransfer);

			bool validate(
				TransactionValidationLevel level = TRANSACTION_VALIDATION_SINGLE, 
				IGradidoBlockchain* blockchain = nullptr,
				const ConfirmedTransaction* parentGradidoBlock = nullptr
			)const;

			bool isBelongToUs(const TransactionBase* pairingTransaction) const;
			Timepoint getTimeoutAsTimePoint() const;
			int64_t getTimeoutAt() const;

		protected:
			const proto::gradido::GradidoDeferredTransfer& mProtoDeferredTransfer;
		};
	}
}



#endif //__GRADIDO_BLOCKCHAIN_PROTOBUF_WRAPPER_DEFERRED_TRANSFER_H