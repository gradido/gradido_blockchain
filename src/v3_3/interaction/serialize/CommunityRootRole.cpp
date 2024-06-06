#include "gradido_blockchain/v3_3/interaction/serialize/CommunityRootRole.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace serialize {
				
				CommunityRootMessage CommunityRootRole::getMessage() const
				{
					return CommunityRootMessage();
				}

				size_t CommunityRootRole::calculateSerializedSize() const
				{
					return 1;
				}
			}
		}
	}
}