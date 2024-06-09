#include "gradido_blockchain/v3_3/interaction/serialize/SignatureMapRole.h"


namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace serialize {

				SignatureMapMessage SignatureMapRole::getMessage() const
				{
					auto& sigPairs = mSignatureMap.signaturePairs;
					SignatureMapMessage signatureMap;
					for (auto it = sigPairs.begin(); it != sigPairs.end(); it++) {
						signatureMap["sig_pair"_f].push_back(
							SignaturePairMessage{
								it->pubkey->copyAsVector(),
								it->signature->copyAsVector()
							}
						);
					}
					return signatureMap;
				}

				size_t SignatureMapRole::calculateSerializedSize() const
				{
					size_t size = mSignatureMap.signaturePairs.size() * (32 + 64 + 6);
					//printf("calculated signature map size: %lld\n", size);
					return size;
				}

			}
		}
	}
}

