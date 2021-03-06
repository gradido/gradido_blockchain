#ifndef __GRADIDO_BLOCKCHAIN_PROTOBUF_WRAPPER_DEFERRED_TRANSFER_H
#define __GRADIDO_BLOCKCHAIN_PROTOBUF_WRAPPER_DEFERRED_TRANSFER_H

#include "TransactionTransfer.h"
#include "proto/gradido/GradidoTransfer.pb.h"

#include "Poco/Timestamp.h"
#include "Poco/DateTime.h"

namespace model {
	namespace gradido {
		class GRADIDOBLOCKCHAIN_EXPORT DeferredTransfer : public TransactionTransfer
		{
		public:
			DeferredTransfer(const proto::gradido::GradidoDeferredTransfer& deferredTransfer);

			bool validate(
				TransactionValidationLevel level = TRANSACTION_VALIDATION_SINGLE, 
				IGradidoBlockchain* blockchain = nullptr,
				const GradidoBlock* parentGradidoBlock = nullptr
			)const;

			bool isBelongToUs(const TransactionBase* pairingTransaction) const;

			Poco::Timestamp getTimeoutAsPocoTimestamp() const;
			inline Poco::DateTime getTimeoutAsPocoDateTime() const { return Poco::DateTime(getTimeoutAsPocoTimestamp()); }
		protected:
			const proto::gradido::GradidoDeferredTransfer& mProtoDeferredTransfer;
		};
	}
}



#endif //__GRADIDO_BLOCKCHAIN_PROTOBUF_WRAPPER_DEFERRED_TRANSFER_H