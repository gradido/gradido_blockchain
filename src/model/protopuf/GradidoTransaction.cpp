#include "gradido_blockchain/model/protopuf/GradidoTransaction.h"

namespace model {
	namespace protopuf {
		GradidoTransaction::GradidoTransaction(const GradidoTransactionMessage& data)
		{
			std::optional<SignatureMapMessage> sigMap = data["sig_map"_f];
			std::optional<std::vector<unsigned char>> bodyBytes = data["body_bytes"_f];
			std::optional<std::vector<unsigned char>> parentMessageId = data["parent_message_id"_f];
			
			if (sigMap.has_value()) {
				mSignatureMap = SignatureMap(sigMap.value());
			}
			if (bodyBytes.has_value()) {
				mBodyBytes = std::make_shared<memory::Block>(bodyBytes.value());
			}
			if (parentMessageId.has_value()) {
				mParentMessageId = std::make_shared<memory::Block>(parentMessageId.value());
			}
		}
		GradidoTransaction::GradidoTransaction(
			const SignaturePair& firstSignaturePair,
			ConstMemoryBlockPtr bodyBytes,
			ConstMemoryBlockPtr parentMessageId /*= nullptr*/
		)
			: mSignatureMap(firstSignaturePair), mBodyBytes(bodyBytes), mParentMessageId(parentMessageId)
		{

		}
	}
}