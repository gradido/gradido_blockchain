#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_HIERO_TOPIC_ID_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_HIERO_TOPIC_ID_ROLE_H

#include "gradido_blockchain/data/hiero/TopicId.h"
#include "gradido_blockchain/export.h"
#include "Protopuf.h"
#include <memory>

namespace gradido {
    namespace interaction {
        namespace deserialize {
            class GRADIDOBLOCKCHAIN_EXPORT HieroTopicIdRole
            {
            public:
                HieroTopicIdRole(const HieroTopicIdMessage& hieroTopicId);
                ~HieroTopicIdRole();

                inline operator const hiero::TopicId& () const { return mTopicId; }
                inline const hiero::TopicId& data() const { return mTopicId; }
            protected:
                hiero::TopicId mTopicId;
            };
        }
    }
}

#endif // __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_HIERO_TOPIC_ID_ROLE_H