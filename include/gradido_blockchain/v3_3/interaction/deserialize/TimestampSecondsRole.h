#ifndef __GRADIDO_BLOCKCHAIN_v3_3_INTERACTION_DESERIALIZE_TIMESTAMP_SECONDS_ROLE_H
#define __GRADIDO_BLOCKCHAIN_v3_3_INTERACTION_DESERIALIZE_TIMESTAMP_SECONDS_ROLE_H

#include "Protopuf.h"
#include "gradido_blockchain/v3_3/data/Protocol.h"

namespace gradido {
    namespace v3_3 {
        namespace interaction {
            namespace deserialize {
                class TimestampSecondsRole 
                {
                public:
                    TimestampSecondsRole(const TimestampSecondsMessage& timestamp);
                    inline operator data::TimestampSeconds() const { return mTimestamp; }
                    inline data::TimestampSeconds data() const { return mTimestamp; }
                protected:
                    data::TimestampSeconds mTimestamp;
                };
            }
        }
    }
}

#endif //__GRADIDO_BLOCKCHAIN_v3_3_INTERACTION_DESERIALIZE_TIMESTAMP_SECONDS_ROLE_H