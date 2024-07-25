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
				for (auto signaturePair : mTransaction.signatureMap.signaturePairs) {
					Value v(kObjectType);
					if (signaturePair.pubkey) {
						v.AddMember("pubkey", Value(signaturePair.pubkey->convertToHex().data(), alloc), alloc);
					}
					if (signaturePair.signature) {
						v.AddMember("signature", Value(signaturePair.signature->convertToHex().data(), alloc), alloc);
					}
					signatures.PushBack(v, alloc);
				}
				d.AddMember("signatureMap", signatures, alloc);
				Value bodyBytes(kObjectType);
				if ((enum_integer(BodyBytesFormat::BASE64) & enum_integer(mFormat)) == enum_integer(BodyBytesFormat::BASE64)) {
					bodyBytes.AddMember("base64", Value(mTransaction.bodyBytes->convertToBase64().data(), alloc), alloc);
				}
				if ((enum_integer(BodyBytesFormat::HEX) & enum_integer(mFormat)) == enum_integer(BodyBytesFormat::HEX)) {
					bodyBytes.AddMember("hex", Value(mTransaction.bodyBytes->convertToHex().data(), alloc), alloc);
				}
				if ((enum_integer(BodyBytesFormat::JSON) & enum_integer(mFormat)) == enum_integer(BodyBytesFormat::JSON)) {
					try {
						TransactionBodyRole bodyBytesToJson(*mTransaction.getTransactionBody());
						bodyBytes.AddMember("json", bodyBytesToJson.composeJson(rootDocument), alloc);
					} catch(std::exception& ex) {
						bodyBytes.AddMember("json", Value(ex.what(), alloc), alloc);
					}
				}
				d.AddMember("bodyBytes", bodyBytes, alloc);
				if (mTransaction.paringMessageId) {
					d.AddMember("paringMessageId", Value(mTransaction.paringMessageId->convertToHex().data(), alloc), alloc);
				}

				return d;
			}
		}
	}
}

