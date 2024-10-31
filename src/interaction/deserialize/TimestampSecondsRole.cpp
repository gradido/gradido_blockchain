#include "gradido_blockchain/data/TimestampSeconds.h"
#include "gradido_blockchain/interaction/deserialize/TimestampSecondsRole.h"
#include "gradido_blockchain/interaction/deserialize/Exceptions.h"

namespace gradido {
    namespace interaction {
        namespace deserialize {
            TimestampSecondsRole::TimestampSecondsRole(const TimestampSecondsMessage& timestamp)
            {
                if (!timestamp["seconds"_f].has_value()) {
					throw MissingMemberException("missing member on deserialize timestamp seconds", "seconds");
                }
                mTimestamp = std::make_unique<data::TimestampSeconds>(timestamp["seconds"_f].value());
            }

            TimestampSecondsRole::~TimestampSecondsRole()
            {
                
            }
        }
    }
}