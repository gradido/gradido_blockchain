#include "gradido_blockchain/interaction/deserialize/GradidoTransactionRole.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

namespace gradido {
	namespace interaction {
		namespace deserialize {
			GradidoTransactionRole::GradidoTransactionRole(const GradidoTransactionMessage& message)
				: mGradidoTransaction(std::make_shared<data::GradidoTransaction>())
			{
				auto sigMap = message["sig_map"_f];
				if (sigMap.has_value() && sigMap.value()["sig_pair"_f].size()) {
					auto sigPairs = sigMap.value()["sig_pair"_f];
					for (int i = 0; i < sigPairs.size(); i++) {
						mGradidoTransaction->signatureMap.push(data::SignaturePair(
							std::make_shared<memory::Block>(sigPairs[i]["pubkey"_f].value()),
							std::make_shared<memory::Block>(sigPairs[i]["signature"_f].value())
						));
					}
				}
				auto bodyBytes = message["body_bytes"_f];
				if (bodyBytes.has_value()) {
					mGradidoTransaction->bodyBytes = std::make_shared<memory::Block>(bodyBytes.value());
				}
				auto paringMessageId = message["parent_message_id"_f];
				if (paringMessageId.has_value()) {
					mGradidoTransaction->paringMessageId = std::make_shared<memory::Block>(paringMessageId.value());
				}
			}
		}
	}
}

