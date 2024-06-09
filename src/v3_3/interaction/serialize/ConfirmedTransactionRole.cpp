#include "gradido_blockchain/v3_3/interaction/serialize/ConfirmedTransactionRole.h"


namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace serialize {

				ConfirmedTransactionMessage ConfirmedTransactionRole::getMessage() const
				{
					assert(mConfirmedTransaction.runningHash);
					assert(mConfirmedTransaction.messageId);
					ConfirmedTransactionMessage confirmedTransactionMessage;
					return ConfirmedTransactionMessage{
						mConfirmedTransaction.id,
						mGradidoTransactionRole.getMessage(),
						TimestampSecondsMessage { mConfirmedTransaction.confirmedAt.seconds },
						mConfirmedTransaction.versionNumber,
						mConfirmedTransaction.runningHash->copyAsVector(),
						mConfirmedTransaction.messageId->copyAsVector(),
						mConfirmedTransaction.accountBalance
					};
					return confirmedTransactionMessage;
				}

				size_t ConfirmedTransactionRole::calculateSerializedSize() const
				{
					size_t size = 8 + 8 
						+ mConfirmedTransaction.versionNumber.size() 
						+ crypto_generichash_BYTES 
						+ 32 
						+ mConfirmedTransaction.accountBalance.size() 
						+ 10;
					size += mGradidoTransactionRole.calculateSerializedSize();
					//printf("calculated confirmed transaction size: %lld\n", size);
					return size;
				}

			}
		}
	}
}
