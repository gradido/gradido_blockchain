#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_TIMESTAMP_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_TIMESTAMP_ROLE_H

#include "Protopuf.h"

namespace gradido {
    namespace data {
        class Timestamp;
    }
    namespace interaction {
        namespace deserialize {
            class TimestampRole 
            {
            public:
                TimestampRole(const TimestampMessage& timestamp);
                inline operator const data::Timestamp&() const { return *mTimestamp.get(); }
                inline const data::Timestamp& data() const { return *mTimestamp.get(); }
            protected:
                std::unique_ptr<data::Timestamp> mTimestamp;
            };
        }
    }
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_TIMESTAMP_ROLE_H