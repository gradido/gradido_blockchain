#ifndef __GRADIDO_BLOCKCHAIN_PROTOBUF_WRAPPER_DEFERRED_TRANSFER_H
#define __GRADIDO_BLOCKCHAIN_PROTOBUF_WRAPPER_DEFERRED_TRANSFER_H

#include "TransactionTransfer.h"
#include "gradido/GradidoTransfer.pb.h"

#include "Poco/Timestamp.h"

namespace model {
	namespace gradido {
		class DeferredTransfer : public TransactionTransfer
		{
		public:
			DeferredTransfer(const proto::gradido::GradidoDeferredTransfer& deferredTransfer);

			bool validate(TransactionValidationLevel level = TRANSACTION_VALIDATION_SINGLE, IGradidoBlockchain* blockchain = nullptr) const;

			Poco::Timestamp getTimeoutAsPocoTimestamp() const;
		protected:
			const proto::gradido::GradidoDeferredTransfer& mProtoDeferredTransfer;
		};
	}
}



#endif //__GRADIDO_BLOCKCHAIN_PROTOBUF_WRAPPER_DEFERRED_TRANSFER_H