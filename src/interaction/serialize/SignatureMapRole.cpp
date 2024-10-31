#include "gradido_blockchain/data/SignatureMap.h"
#include "gradido_blockchain/interaction/serialize/SignatureMapRole.h"

namespace gradido {
	namespace interaction {
		namespace serialize {

			SignatureMapMessage SignatureMapRole::getMessage() const
			{
				auto& sigPairs = mSignatureMap.getSignaturePairs();
				SignatureMapMessage signatureMap;
				for (auto it = sigPairs.begin(); it != sigPairs.end(); it++) {
					signatureMap["sig_pair"_f].push_back(
						SignaturePairMessage{
							it->getPubkey()->copyAsVector(),
							it->getSignature()->copyAsVector()
						}
					);
				}
				return signatureMap;
			}

			size_t SignatureMapRole::calculateSerializedSize() const
			{
				size_t size = mSignatureMap.getSignaturePairs().size() * (32 + 64 + 6);
				//printf("calculated signature map size: %lld\n", size);
				return size;
			}

		}
	}
}

