#include "gradido_blockchain/data/CommunityRoot.h"

namespace gradido {
	namespace data {
		bool CommunityRoot::isInvolved(memory::ConstBlockPtr publicKey) const
		{
			return
				publicKey->isTheSame(mPubkey) ||
				publicKey->isTheSame(mGmwPubkey) ||
				publicKey->isTheSame(mAufPubkey);
		}
	}
}