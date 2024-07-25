#include "gradido_blockchain/interaction/serialize/GradidoTransactionRole.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

namespace gradido {
	namespace interaction {
		namespace serialize {

			GradidoTransactionMessage GradidoTransactionRole::getMessage() const
			{
				GradidoTransactionMessage gradidoTransactionMessage;
					
				if (mGradidoTransaction.bodyBytes) {
					gradidoTransactionMessage["body_bytes"_f] = mGradidoTransaction.bodyBytes->copyAsVector();
				}
				if (mGradidoTransaction.paringMessageId) {
					gradidoTransactionMessage["parent_message_id"_f] = mGradidoTransaction.paringMessageId->copyAsVector();
				}					
				gradidoTransactionMessage["sig_map"_f] = mSigantureMapRole.getMessage();
				return gradidoTransactionMessage;
			}

			size_t GradidoTransactionRole::calculateSerializedSize() const
			{
				size_t size = mSigantureMapRole.calculateSerializedSize();
				if (mGradidoTransaction.bodyBytes) {
					size += mGradidoTransaction.bodyBytes->size();
				}
				if (mGradidoTransaction.paringMessageId) {
					size += mGradidoTransaction.paringMessageId->size();
				}
				return size;
			}

		}
	}
}

