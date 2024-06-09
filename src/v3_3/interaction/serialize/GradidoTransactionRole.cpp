#include "gradido_blockchain/v3_3/interaction/serialize/GradidoTransactionRole.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace serialize {

				data::GradidoTransactionMessage GradidoTransactionRole::getMessage() const
				{
					data::GradidoTransactionMessage gradidoTransactionMessage;
					
					if (mGradidoTransaction.bodyBytes) {
						gradidoTransactionMessage["body_bytes"_f] = mGradidoTransaction.bodyBytes->copyAsVector();
					}
					if (mGradidoTransaction.parentMessageId) {
						gradidoTransactionMessage["parent_message_id"_f] = mGradidoTransaction.parentMessageId->copyAsVector();
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
					if (mGradidoTransaction.parentMessageId) {
						size += mGradidoTransaction.parentMessageId->size();
					}
					return size;
				}

			}
		}
	}
}

