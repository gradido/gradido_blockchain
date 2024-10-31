#include "gradido_blockchain/data/Timestamp.h"
#include "gradido_blockchain/interaction/deserialize/TimestampRole.h"
#include "gradido_blockchain/interaction/deserialize/Exceptions.h"

namespace gradido {
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