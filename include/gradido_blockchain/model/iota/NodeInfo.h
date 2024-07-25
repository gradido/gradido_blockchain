#ifndef __GRADIDO_BLOCKCHAIN_MODEL_IOTA_NODE_INFO_H
#define __GRADIDO_BLOCKCHAIN_MODEL_IOTA_NODE_INFO_H

#include "gradido_blockchain/export.h"

#include <string>

namespace iota {
    struct GRADIDOBLOCKCHAIN_EXPORT NodeInfo
	{
		std::string version;
		bool isHealthy;
		float messagesPerSecond;
		int32_t lastMilestoneIndex;
		int64_t lastMilestoneTimestamp;
		int32_t confirmedMilestoneIndex;
	};
}

#endif //__GRADIDO_BLOCKCHAIN_MODEL_IOTA_NODE_INFO_H