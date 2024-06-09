#ifndef __GRADIDO_BLOCKCHAIN_v3_3_INTERACTION_DESERIALIZE_TIMESTAMP_ROLE_H
#define __GRADIDO_BLOCKCHAIN_v3_3_INTERACTION_DESERIALIZE_TIMESTAMP_ROLE_H

#include "Protopuf.h"
#include "gradido_blockchain/v3_3/data/Protocol.h"

namespace gradido {
    namespace v3_3 {
        namespace interaction {
            namespace deserialize {
                class TimestampRole 
                {
                public:
                    TimestampRole(const TimestampMessage& timestamp);
                    inline operator data::Timestamp() const { return mTimestamp; }
                    inline data::Timestamp data() const { return mTimestamp; }
                protected:
                    data::Timestamp mTimestamp;
                };
            }
        }
    }
}

#endif //__GRADIDO_BLOCKCHAIN_v3_3_INTERACTION_DESERIALIZE_TIMESTAMP_ROLE_H