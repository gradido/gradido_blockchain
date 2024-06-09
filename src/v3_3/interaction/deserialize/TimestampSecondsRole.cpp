#include "gradido_blockchain/v3_3/interaction/deserialize/TimestampSecondsRole.h"

namespace gradido {
    namespace v3_3 {
        namespace interaction {
            namespace deserialize {
                TimestampSecondsRole::TimestampSecondsRole(const TimestampSecondsMessage& timestamp)
                : mTimestamp(timestamp["seconds"_f].value()) {}
            }
        }
    }
}