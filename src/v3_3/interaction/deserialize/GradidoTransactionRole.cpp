#include "gradido_blockchain/v3_3/interaction/deserialize/GradidoTransactionRole.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace deserialize {
				GradidoTransactionRole::GradidoTransactionRole(const GradidoTransactionMessage& message)
					: mGradidoTransaction(std::make_unique<data::GradidoTransaction>())
				{
					auto sigMap = message["sig_map"_f];
					std::optional<std::vector<unsigned char>> bodyBytes = message["body_bytes"_f];
					std::optional<std::vector<unsigned char>> parentMessageId = message["parent_message_id"_f];

					if (sigMap.has_value() && sigMap.value()["sig_pair"_f].size()) {
						auto sigPairs = sigMap.value()["sig_pair"_f];
						for (int i = 0; i < sigPairs.size(); i++) {
							mGradidoTransaction->signatureMap.push(data::SignaturePair(
								std::make_shared<memory::Block>(sigPairs[i]["pubkey"_f].value()),
								std::make_shared<memory::Block>(sigPairs[i]["signature"_f].value())
							));
						}
					}
					if (bodyBytes.has_value()) {
						mGradidoTransaction->bodyBytes = std::make_shared<memory::Block>(bodyBytes.value());
					}
					if (parentMessageId.has_value()) {
						mGradidoTransaction->parentMessageId = std::make_shared<memory::Block>(parentMessageId.value());
					}
				}
			}
		}
	}
}

