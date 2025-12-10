#include "gradido_blockchain/data/GradidoTransaction.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/interaction/deserialize/GradidoTransactionRole.h"

namespace gradido {
	namespace interaction {
		namespace deserialize {
			GradidoTransactionRole::~GradidoTransactionRole()
			{

			}
			
			GradidoTransactionRole::GradidoTransactionRole(const GradidoTransactionMessage& message)
			{
				data::SignatureMap signatures;
				memory::BlockPtr bodyBytesPtr;
				memory::BlockPtr paringMessageIdPtr = nullptr;
				
				const auto& sigMap = &message["sig_map"_f];
				if (sigMap->has_value() && sigMap->value()["sig_pair"_f].size()) {
					const auto& sigPairs = &sigMap->value()["sig_pair"_f];
					signatures.reserve(sigPairs->size());
					for (int i = 0; i < sigPairs->size(); i++) {
						signatures.push(data::SignaturePair(
							std::make_shared<memory::Block>((*sigPairs)[i]["pubkey"_f].value()),
							std::make_shared<memory::Block>((*sigPairs)[i]["signature"_f].value())
						));
					}
				}
				const auto& bodyBytes = &message["body_bytes"_f];
				if (bodyBytes->has_value()) {
					bodyBytesPtr = std::make_shared<memory::Block>(bodyBytes->value());
				}
				const auto& paringMessageId = &message["parent_message_id"_f];
				if (paringMessageId->has_value()) {
					paringMessageIdPtr = std::make_shared<memory::Block>(paringMessageId->value());
				}
				mGradidoTransaction = std::make_unique<const data::GradidoTransaction>(signatures, bodyBytesPtr, paringMessageIdPtr);
			}
		}
	}
}

