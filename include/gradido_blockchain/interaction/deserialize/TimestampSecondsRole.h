#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_TIMESTAMP_SECONDS_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_TIMESTAMP_SECONDS_ROLE_H

#include "Protopuf.h"
#include "gradido_blockchain/data/Protocol.h"

namespace gradido {
    namespace interaction {
        namespace deserialize {
            class TimestampSecondsRole 
            {
            public:
                TimestampSecondsRole(const TimestampSecondsMessage& timestamp);
                inline operator const data::TimestampSeconds&() const { return *mTimestamp.get(); }
                inline const data::TimestampSeconds& data() const { return *mTimestamp.get(); }
            protected:
                std::unique_ptr<data::TimestampSeconds> mTimestamp;
            };
        }
    }
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_TIMESTAMP_SECONDS_ROLE_H