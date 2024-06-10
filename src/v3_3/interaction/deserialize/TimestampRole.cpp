#include "gradido_blockchain/v3_3/interaction/deserialize/TimestampRole.h"
#include "gradido_blockchain/v3_3/interaction/deserialize/Exceptions.h"

namespace gradido {
    namespace v3_3 {
        namespace interaction {
            namespace deserialize {
                TimestampRole::TimestampRole(const TimestampMessage& timestamp)
                {
                    const char* exceptionMessage = "missing member on deserialize timestamp seconds";
                    if (!timestamp["seconds"_f].has_value()) {
                        throw MissingMemberException(exceptionMessage, "seconds");
                    }
					if (!timestamp["nanos"_f].has_value()) {
						throw MissingMemberException(exceptionMessage, "nanos");
					}
                    mTimestamp = std::make_unique<data::Timestamp>(timestamp["seconds"_f].value(), timestamp["nanos"_f].value());
                }
            }
        }
    }
}