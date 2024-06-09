#include "gradido_blockchain/v3_3/interaction/deserialize/TimestampRole.h"

namespace gradido {
    namespace v3_3 {
        namespace interaction {
            namespace deserialize {
                TimestampRole::TimestampRole(const TimestampMessage& timestamp)
                : mTimestamp(timestamp["seconds"_f].value(), timestamp["nanos"_f].value()) {}
            }
        }
    }
}