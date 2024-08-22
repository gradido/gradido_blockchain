#include "gradido_blockchain/interaction/serialize/GradidoTransactionRole.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

namespace gradido {
	namespace interaction {
		namespace serialize {

			GradidoTransactionMessage GradidoTransactionRole::getMessage() const
			{
				GradidoTransactionMessage gradidoTransactionMessage;
					
				if (mGradidoTransaction.getBodyBytes()) {
					gradidoTransactionMessage["body_bytes"_f] = mGradidoTransaction.getBodyBytes()->copyAsVector();
				}
				if (mGradidoTransaction.getParingMessageId()) {
					gradidoTransactionMessage["parent_message_id"_f] = mGradidoTransaction.getParingMessageId()->copyAsVector();
				}					
				gradidoTransactionMessage["sig_map"_f] = mSigantureMapRole.getMessage();
				return gradidoTransactionMessage;
			}

			size_t GradidoTransactionRole::calculateSerializedSize() const
			{
				size_t size = mSigantureMapRole.calculateSerializedSize();
				if (mGradidoTransaction.getBodyBytes()) {
					size += mGradidoTransaction.getBodyBytes()->size();
				}
				if (mGradidoTransaction.getParingMessageId()) {
					size += mGradidoTransaction.getParingMessageId()->size();
				}
				return size;
			}

		}
	}
}

