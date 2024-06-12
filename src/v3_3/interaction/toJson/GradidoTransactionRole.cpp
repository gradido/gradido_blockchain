#include "gradido_blockchain/v3_3/interaction/toJson/GradidoTransactionRole.h"
#include "gradido_blockchain/v3_3/interaction/deserialize/Context.h"
#include "gradido_blockchain/v3_3/interaction/toJson/Context.h"

#include "magic_enum/magic_enum.hpp"
#include "magic_enum/magic_enum_flags.hpp"

using namespace rapidjson;
using namespace magic_enum;

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace toJson {
				const char* GradidoTransactionRole::run(bool pretty) const
				{
					Document d(kObjectType);
					auto& alloc = d.GetAllocator();
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
						deserialize::Context deserializeBodyBytes(mTransaction.bodyBytes, deserialize::Type::TRANSACTION_BODY);
						deserializeBodyBytes.run();
						if (deserializeBodyBytes.isTransactionBody()) {
							auto transactionBody = deserializeBodyBytes.getTransactionBody();
							toJson::Context bodyBytesToJson(*transactionBody.get());
							bodyBytes.AddMember("json", Value(bodyBytesToJson.run(pretty), alloc), alloc);
						}
						else {
							bodyBytes.AddMember("json", Value("invalid body bytes", alloc), alloc);
						}
					}
					d.AddMember("bodyBytes", bodyBytes, alloc);
					if (mTransaction.parentMessageId) {
						d.AddMember("parentMessageId", Value(mTransaction.parentMessageId->convertToHex().data(), alloc), alloc);
					}

					return toString(&d, pretty);
				}
			}
		}
	}
}

