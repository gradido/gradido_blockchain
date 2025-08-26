#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_HIERO_TOPIC_ID_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_HIERO_TOPIC_ID_ROLE_H

#include "AbstractRole.h"
#include "Protopuf.h"

namespace hiero {
	class TopicId;
}

namespace gradido {
	namespace interaction {
		namespace serialize {

			class HieroTopicIdRole : public AbstractRole
			{
			public:
				HieroTopicIdRole(const hiero::TopicId& accountId) : mTopicId(accountId) {}
				~HieroTopicIdRole() {};

				RUM_IMPLEMENTATION
				HieroTopicIdMessage getMessage() const;
				size_t calculateSerializedSize() const;

			protected:
				const hiero::TopicId& mTopicId;
			};
		}
	}
}

#endif // __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_HIERO_TOPIC_ID_ROLE_H