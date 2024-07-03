#include "gradido_blockchain/interaction/serialize/ConfirmedTransactionRole.h"
#include "gradido_blockchain/interaction/serialize/Exceptions.h"

namespace gradido {
	namespace interaction {
		namespace serialize {

			ConfirmedTransactionMessage ConfirmedTransactionRole::getMessage() const
			{
				if (!mConfirmedTransaction.runningHash) {
					throw MissingMemberException("missing member by serializing ConfirmedTransaction", "runningHash");
				}
				if (!mConfirmedTransaction.messageId) {
					throw MissingMemberException("missing member by serializing ConfirmedTransaction", "messageId");
				}
				ConfirmedTransactionMessage confirmedTransactionMessage;
				return ConfirmedTransactionMessage{
					mConfirmedTransaction.id,
					mGradidoTransactionRole.getMessage(),
					TimestampSecondsMessage { mConfirmedTransaction.confirmedAt.seconds },
					mConfirmedTransaction.versionNumber,
					mConfirmedTransaction.runningHash->copyAsVector(),
					mConfirmedTransaction.messageId->copyAsVector(),
					decimalToStringTrimTrailingZeros(mConfirmedTransaction.accountBalance)
				};
				return confirmedTransactionMessage;
			}

			size_t ConfirmedTransactionRole::calculateSerializedSize() const
			{
				size_t size = 8 + 8 
					+ mConfirmedTransaction.versionNumber.size() 
					+ crypto_generichash_BYTES 
					+ 32 
					+ mConfirmedTransaction.accountBalance.toString().size() 
					+ 10;
				size += mGradidoTransactionRole.calculateSerializedSize();
				//printf("calculated confirmed transaction size: %lld\n", size);
				return size;
			}

		}
	}
}
