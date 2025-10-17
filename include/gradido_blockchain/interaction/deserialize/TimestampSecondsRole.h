#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_TIMESTAMP_SECONDS_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_TIMESTAMP_SECONDS_ROLE_H

#include "gradido_blockchain/export.h"
#include "Protopuf.h"
#include <memory>

namespace gradido {
    namespace data {
        class TimestampSeconds;
    }
    namespace interaction {
        namespace deserialize {
            class GRADIDOBLOCKCHAIN_EXPORT TimestampSecondsRole
            {
            public:
                TimestampSecondsRole(const TimestampSecondsMessage& timestamp);
                ~TimestampSecondsRole();
                inline operator const data::TimestampSeconds&() const { return *mTimestamp.get(); }
                inline const data::TimestampSeconds& data() const { return *mTimestamp.get(); }
            protected:
                std::unique_ptr<data::TimestampSeconds> mTimestamp;
            };
        }
    }
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_TIMESTAMP_SECONDS_ROLE_H