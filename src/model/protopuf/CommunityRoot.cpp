#include "gradido_blockchain/model/protopuf/CommunityRoot.h"

namespace model {
	namespace protopuf {
		CommunityRoot::CommunityRoot(const CommunityRootMessage& data)
			: mPubkey(std::make_shared<CachedMemoryBlock>(data["pubkey"_f].value())),
			mGmwPubkey(std::make_shared<CachedMemoryBlock>(data["gmw_pubkey"_f].value())),
			mAufPubkey(std::make_shared<CachedMemoryBlock>(data["auf_pubkey"_f].value()))
		{
		}

		CommunityRoot::CommunityRoot(
			ConstCachedMemoryBlockPtr pubkey,
			ConstCachedMemoryBlockPtr gmwPubkey,
			ConstCachedMemoryBlockPtr aufPubkey
		)
			: mPubkey(pubkey), mGmwPubkey(gmwPubkey), mAufPubkey(aufPubkey)
		{
		}
	}
}