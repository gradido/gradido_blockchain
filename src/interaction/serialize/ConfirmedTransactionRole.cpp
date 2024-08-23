#include "gradido_blockchain/interaction/serialize/ConfirmedTransactionRole.h"
#include "gradido_blockchain/interaction/serialize/Exceptions.h"

namespace gradido {
	namespace interaction {
		namespace serialize {

			ConfirmedTransactionMessage ConfirmedTransactionRole::getMessage() const
			{
				if (!mConfirmedTransaction.getRunningHash()) {
					throw MissingMemberException("missing member by serializing ConfirmedTransaction", "runningHash");
				}
				if (!mConfirmedTransaction.getMessageId()) {
					throw MissingMemberException("missing member by serializing ConfirmedTransaction", "messageId");
				}
				ConfirmedTransactionMessage confirmedTransactionMessage;
				return ConfirmedTransactionMessage{
					mConfirmedTransaction.getId(),
					mGradidoTransactionRole.getMessage(),
					TimestampSecondsMessage { mConfirmedTransaction.getConfirmedAt().getSeconds()},
					mConfirmedTransaction.getVersionNumber(),
					mConfirmedTransaction.getRunningHash()->copyAsVector(),
					mConfirmedTransaction.getMessageId()->copyAsVector(),
					GradidoUnitToStringTrimTrailingZeros(mConfirmedTransaction.getAccountBalance())
				};
				return confirmedTransactionMessage;
			}

			size_t ConfirmedTransactionRole::calculateSerializedSize() const
			{
				size_t size = 8 + 8 
					+ mConfirmedTransaction.getVersionNumber().size()
					+ crypto_generichash_BYTES 
					+ 32 
					+ mConfirmedTransaction.getAccountBalance().toString().size()
					+ 10;
				size += mGradidoTransactionRole.calculateSerializedSize();
				//printf("calculated confirmed transaction size: %lld\n", size);
				return size;
			}

		}
	}
}
