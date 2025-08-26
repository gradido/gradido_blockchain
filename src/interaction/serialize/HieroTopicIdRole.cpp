#include "gradido_blockchain/interaction/serialize/HieroTopicIdRole.h"
#include "gradido_blockchain/data/hiero/TopicId.h"

namespace gradido {
	namespace interaction {
		namespace serialize {
			HieroTopicIdMessage HieroTopicIdRole::getMessage() const
			{
				HieroTopicIdMessage message;
				message["shardNum"_f] = mTopicId.getShardNum();
				message["realmNum"_f] = mTopicId.getRealmNum();
				message["topicNum"_f] = mTopicId.getTopicNum();
				return message;
			}

			size_t HieroTopicIdRole::calculateSerializedSize() const
			{
				return 3 * 9 + 2;
			}
		}
	}
}