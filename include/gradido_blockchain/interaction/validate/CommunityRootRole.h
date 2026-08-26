#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_COMMUNITY_ROOT_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_COMMUNITY_ROOT_ROLE_H

#include "AbstractRole.h"
#include "gradido_blockchain/data/compact/CommunityRootTx.h"

namespace gradido {
	namespace interaction {
		namespace validate {
			class CommunityRootRole : public AbstractRole
			{
			public:
				CommunityRootRole(data::compact::CommunityRootTx&& communityRoot, uint32_t communityIdIndex);

				void run(Type type, ContextData& c);
			protected:
				data::compact::CommunityRootTx mCommunityRoot;
				uint32_t mCommunityIdIndex;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_COMMUNITY_ROOT_ROLE_H