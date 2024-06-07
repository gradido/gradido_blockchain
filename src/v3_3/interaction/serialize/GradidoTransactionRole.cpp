#include "gradido_blockchain/v3_3/interaction/serialize/GradidoTransactionRole.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace serialize {

				data::GradidoTransactionMessage GradidoTransactionRole::getMessage() const
				{
					data::GradidoTransactionMessage gradidoTransactionMessage;
					
					if (mGradidoTransaction.mBodyBytes) {
						gradidoTransactionMessage["body_bytes"_f] = mGradidoTransaction.mBodyBytes->copyAsVector();
					}
					if (mGradidoTransaction.mParentMessageId) {
						gradidoTransactionMessage["parent_message_id"_f] = mGradidoTransaction.mParentMessageId->copyAsVector();
					}					

					auto& sigPairs = mGradidoTransaction.mSignatureMap.signaturePairs;
					data::SignatureMapMessage signatureMap;
					for (auto it = sigPairs.begin(); it != sigPairs.end(); it++) {
						signatureMap["sig_pair"_f].push_back(
							data::SignaturePairMessage {
								it->pubkey->copyAsVector(),
								it->signature->copyAsVector()
							}
						);
					}
					gradidoTransactionMessage["sig_map"_f] = signatureMap;
					return gradidoTransactionMessage;
				}

				size_t GradidoTransactionRole::calculateSerializedSize() const
				{
					size_t size = mGradidoTransaction.mSignatureMap.signaturePairs.size() * (32 + 64) + 10;
					if (mGradidoTransaction.mBodyBytes) {
						size += mGradidoTransaction.mBodyBytes->size();
					}
					if (mGradidoTransaction.mParentMessageId) {
						size += mGradidoTransaction.mParentMessageId->size();
					}
					return size;
				}

			}
		}
	}
}

