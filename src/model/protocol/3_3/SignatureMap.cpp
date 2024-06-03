#include "gradido_blockchain/model/protopuf/SignatureMap.h"

namespace model {
	namespace protopuf {
		
		SignatureMap::SignatureMap(const SignatureMapMessage& data)
		{
			auto maxCount = data["sig_pair"_f].size();
			for (auto i = 0; i < maxCount; i++) {
				mSignaturePairs.push_back(std::move(SignaturePair(data["sig_pair"_f][i])));
			}
			
		}

		SignatureMap::SignatureMap(SignaturePair firstSignaturePair)
		{
			addSignaturePair(std::move(firstSignaturePair));
		}		
	}
}