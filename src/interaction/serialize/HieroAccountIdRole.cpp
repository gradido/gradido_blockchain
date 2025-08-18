#include "gradido_blockchain/interaction/serialize/HieroAccountIdRole.h"
#include "gradido_blockchain/data/hiero/AccountId.h"

namespace gradido {
	namespace interaction {
		namespace serialize {
			HieroAccountIdMessage HieroAccountIdRole::getMessage() const
			{
				HieroAccountIdMessage message;
				message["shardNum"_f] = mAccountId.getShardNum();
				message["realmNum"_f] = mAccountId.getRealmNum();
				if (!mAccountId.getAlias().isEmpty()) {
					message["alias"_f] = mAccountId.getAlias().copyAsVector();
				}
				else {
					message["accountNum"_f] = mAccountId.getAccountNum();
				}
				return message;
			}

			size_t HieroAccountIdRole::calculateSerializedSize() const
			{
				size_t size = 2 * 9 + 2;
				if (!mAccountId.getAlias().isEmpty()) {
					size += mAccountId.getAlias().size();
				}
				else {
					size += 9;
				}
				//printf("calculated signature map size: %lld\n", size);
				return size;
			}
		}
	}
}