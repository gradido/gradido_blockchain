#include "gradido_blockchain/data/GradidoTransaction.h"
#include "gradido_blockchain/interaction/toJson/GradidoTransactionRole.h"
#include "gradido_blockchain/interaction/toJson/TransactionBodyRole.h"

#include "magic_enum/magic_enum.hpp"
#include "magic_enum/magic_enum_flags.hpp"

using namespace rapidjson;
using namespace magic_enum;

namespace gradido {
	namespace interaction {
		namespace toJson {
			Value GradidoTransactionRole::composeJson(rapidjson::Document& rootDocument) const
			{
				Value d(kObjectType);
				auto& alloc = rootDocument.GetAllocator();
				Value signatures(kArrayType);
				for (auto signaturePair : mTransaction.getSignatureMap().getSignaturePairs()) {
					Value v(kObjectType);
					if (signaturePair.getPubkey()) {
						v.AddMember("pubkey", Value(signaturePair.getPubkey()->convertToHex().data(), alloc), alloc);
					}
					if (signaturePair.getSignature()) {
						v.AddMember("signature", Value(signaturePair.getSignature()->convertToHex().data(), alloc), alloc);
					}
					signatures.PushBack(v, alloc);
				}
				d.AddMember("signatureMap", signatures, alloc);
				Value bodyBytes(kObjectType);
				if ((enum_integer(BodyBytesType::BASE64) & enum_integer(mFormat)) == enum_integer(BodyBytesType::BASE64)) {
					bodyBytes.AddMember("base64", Value(mTransaction.getBodyBytes()->convertToBase64().data(), alloc), alloc);
				}
				if ((enum_integer(BodyBytesType::HEX) & enum_integer(mFormat)) == enum_integer(BodyBytesType::HEX)) {
					bodyBytes.AddMember("hex", Value(mTransaction.getBodyBytes()->convertToHex().data(), alloc), alloc);
				}
				if ((enum_integer(BodyBytesType::JSON) & enum_integer(mFormat)) == enum_integer(BodyBytesType::JSON)) {
					try {
						TransactionBodyRole bodyBytesToJson(*mTransaction.getTransactionBody());
						bodyBytes.AddMember("json", bodyBytesToJson.composeJson(rootDocument), alloc);
					} catch(std::exception& ex) {
						bodyBytes.AddMember("json", Value(ex.what(), alloc), alloc);
					}
				}
				d.AddMember("bodyBytes", bodyBytes, alloc);
				if (mTransaction.getParingMessageId()) {
					d.AddMember("paringMessageId", Value(mTransaction.getParingMessageId()->convertToHex().data(), alloc), alloc);
				}

				return d;
			}
		}
	}
}

