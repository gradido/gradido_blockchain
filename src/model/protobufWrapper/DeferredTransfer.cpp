#include "gradido_blockchain/model/protobufWrapper/DeferredTransfer.h"
#include "gradido_blockchain/model/protobufWrapper/ConfirmedTransaction.h"
#include "gradido_blockchain/model/protobufWrapper/TransactionValidationExceptions.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"
#include "gradido_blockchain/lib/Decay.h"

namespace model {
	namespace gradido {
		DeferredTransfer::DeferredTransfer(const proto::gradido::GradidoDeferredTransfer& deferredTransfer)
			: TransactionTransfer(deferredTransfer.transfer()), mProtoDeferredTransfer(deferredTransfer)
		{

		}

		bool DeferredTransfer::validate(
			TransactionValidationLevel level/* = TRANSACTION_VALIDATION_SINGLE*/,
			IGradidoBlockchain* blockchain/* = nullptr*/,
			const ConfirmedTransaction* parentConfirmedTransaction/* = nullptr*/
		) const
		{
			if ((level & TRANSACTION_VALIDATION_SINGLE) == TRANSACTION_VALIDATION_SINGLE) {
				if (parentConfirmedTransaction) {
					if (parentConfirmedTransaction->getConfirmedAtAsTimestamp() >= getTimeoutAsPocoTimestamp()) {
						throw TransactionValidationInvalidInputException("timeout is already in the past", "timeout", "timestamp");
					}
				}
			}
			return TransactionTransfer::validate(level, blockchain, parentConfirmedTransaction);
		}

		bool DeferredTransfer::isBelongToUs(const TransactionBase* pairingTransaction) const
		{
			auto pair = dynamic_cast<const DeferredTransfer*>(pairingTransaction);
			if (getTimeoutAsPocoTimestamp() == pair->getTimeoutAsPocoTimestamp()) {
				return TransactionTransfer::isBelongToUs(pairingTransaction);
			}
			else {
				return false;
			}
		}

		Poco::Timestamp DeferredTransfer::getTimeoutAsPocoTimestamp() const
		{
			return DataTypeConverter::convertFromProtoTimestampSeconds(mProtoDeferredTransfer.timeout());
		}
	}
}
