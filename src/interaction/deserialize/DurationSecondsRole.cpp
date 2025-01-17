#include "gradido_blockchain/data/DurationSeconds.h"
#include "gradido_blockchain/interaction/deserialize/DurationSecondsRole.h"
#include "gradido_blockchain/interaction/deserialize/Exceptions.h"

namespace gradido {
    namespace interaction {
        namespace deserialize {
            DurationSecondsRole::DurationSecondsRole(const DurationSecondsMessage& duration)
            {
                if (!duration["seconds"_f].has_value()) {
                    throw MissingMemberException("missing member on deserialize duration seconds", "seconds");
                }
                mDuration = std::make_unique<data::DurationSeconds>(duration["seconds"_f].value());
            }

            DurationSecondsRole::~DurationSecondsRole()
            {

            }
        }
    }
}