#include "gradido_blockchain/interaction/deserialize/HieroTopicIdRole.h"
#include "gradido_blockchain/interaction/deserialize/Exceptions.h"

namespace gradido {
    namespace interaction {
        namespace deserialize {
            HieroTopicIdRole::HieroTopicIdRole(const HieroTopicIdMessage& hieroTopicId)
            {
                int64_t shardNum = 0;
                int64_t realmNum = 0;
                if (hieroTopicId["shardNum"_f].has_value()) {
                    shardNum = hieroTopicId["shardNum"_f].value();
                }
                if (hieroTopicId["realmNum"_f].has_value()) {
                    realmNum = hieroTopicId["realmNum"_f].value();
                }
                if (hieroTopicId["topicNum"_f].has_value()) {
                    mTopicId = hiero::TopicId(shardNum, realmNum, hieroTopicId["topicNum"_f].value());
                }
                else {
                    throw MissingMemberException("either accountNum or alias must be set", "hiero::AccountId");
                }
            }

            HieroTopicIdRole::~HieroTopicIdRole()
            {

            }
        }
    }
}