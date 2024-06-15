#ifndef __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_COMMUNITY_ROOT_ROLE_H
#define __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_COMMUNITY_ROOT_ROLE_H

#include "AbstractRole.h"
#include "gradido_blockchain/v3_3/data/Protocol.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace validate {
				class CommunityRootRole : public AbstractRole
				{
				public:
					CommunityRootRole(const data::CommunityRoot& communityRoot)
						: mCommunityRoot(communityRoot) {}

					void run(
						Type type,
						const std::string& communityId,
						std::shared_ptr<AbstractBlockchainProvider> blockchainProvider,
						data::ConfirmedTransactionPtr previousConfirmedTransaction,
						data::ConfirmedTransactionPtr recipientPreviousConfirmedTransaction
					);
				protected:

					const data::CommunityRoot& mCommunityRoot;
				};
			}
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_COMMUNITY_ROOT_ROLE_H