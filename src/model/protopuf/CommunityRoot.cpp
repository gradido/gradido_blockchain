#include "gradido_blockchain/model/protopuf/CommunityRoot.h"

namespace model {
	namespace protopuf {
		CommunityRoot::CommunityRoot(const CommunityRootMessage& data)
			: mPubkey(std::make_shared<memory::Block>(data["pubkey"_f].value())),
			mGmwPubkey(std::make_shared<memory::Block>(data["gmw_pubkey"_f].value())),
			mAufPubkey(std::make_shared<memory::Block>(data["auf_pubkey"_f].value()))
		{
		}

		CommunityRoot::CommunityRoot(
			ConstMemoryBlockPtr pubkey,
			ConstMemoryBlockPtr gmwPubkey,
			ConstMemoryBlockPtr aufPubkey
		)
			: mPubkey(pubkey), mGmwPubkey(gmwPubkey), mAufPubkey(aufPubkey)
		{
		}
	}
}