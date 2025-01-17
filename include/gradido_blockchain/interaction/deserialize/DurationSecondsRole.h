#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_DURATION_SECONDS_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_DURATION_SECONDS_ROLE_H

#include "Protopuf.h"
#include <memory>

namespace gradido {
    namespace data {
        class DurationSeconds;
    }
    namespace interaction {
        namespace deserialize {
            class DurationSecondsRole
            {
            public:
                DurationSecondsRole(const DurationSecondsMessage& duration);
                ~DurationSecondsRole();
                inline operator const data::DurationSeconds& () const { return *mDuration.get(); }
                inline const data::DurationSeconds& data() const { return *mDuration.get(); }
            protected:
                std::unique_ptr<data::DurationSeconds> mDuration;
            };
        }
    }
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_DURATION_SECONDS_ROLE_H