#include "gradido_blockchain/model/protobufWrapper/DeferredTransfer.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

namespace model {
	namespace gradido {
		DeferredTransfer::DeferredTransfer(const proto::gradido::GradidoDeferredTransfer& deferredTransfer)
			: TransactionTransfer(deferredTransfer.transfer()), mProtoDeferredTransfer(deferredTransfer)
		{

		}

		bool DeferredTransfer::validate(TransactionValidationLevel level/* = TRANSACTION_VALIDATION_SINGLE*/, IGradidoBlockchain* blockchain/* = nullptr*/) const
		{
			return TransactionTransfer::validate(level, blockchain);
		}

		Poco::Timestamp DeferredTransfer::getTimeoutAsPocoTimestamp() const
		{
			return DataTypeConverter::convertFromProtoTimestamp(mProtoDeferredTransfer.timeout());
		}
	}
}
