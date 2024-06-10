#include "gradido_blockchain/v3_3/interaction/deserialize/TimestampSecondsRole.h"
#include "gradido_blockchain/v3_3/interaction/deserialize/Exceptions.h"

namespace gradido {
    namespace v3_3 {
        namespace interaction {
            namespace deserialize {
                TimestampSecondsRole::TimestampSecondsRole(const TimestampSecondsMessage& timestamp)
                {
                    if (!timestamp["seconds"_f].has_value()) {
						throw MissingMemberException("missing member on deserialize timestamp seconds", "seconds");
                    }
                    mTimestamp = std::make_unique<data::TimestampSeconds>(timestamp["seconds"_f].value());
                }
            }
        }
    }
}