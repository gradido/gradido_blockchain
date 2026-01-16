#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_COMMUNITY_ROOT_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_COMMUNITY_ROOT_ROLE_H

#include "AbstractRole.h"

namespace gradido {
	namespace interaction {
		namespace validate {
			class CommunityRootRole : public AbstractRole
			{
			public:
				CommunityRootRole(std::shared_ptr<const data::CommunityRoot> communityRoot);

				void run(
					Type type,
					std::shared_ptr<blockchain::Abstract> blockchain,
					std::shared_ptr<const data::ConfirmedTransaction> ownBlockchainPreviousConfirmedTransaction,
					std::shared_ptr<const data::ConfirmedTransaction> otherBlockchainPreviousConfirmedTransaction
				);
			protected:
				std::shared_ptr<const data::CommunityRoot> mCommunityRoot;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_COMMUNITY_ROOT_ROLE_H