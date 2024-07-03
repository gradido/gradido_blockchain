#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_COMMUNITY_ROOT_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_COMMUNITY_ROOT_ROLE_H

#include "AbstractRole.h"
#include "gradido_blockchain/data/Protocol.h"

namespace gradido {
	namespace interaction {
		namespace validate {
			class CommunityRootRole : public AbstractRole
			{
			public:
				CommunityRootRole(const data::CommunityRoot& communityRoot);

				void run(
					Type type,
					std::string_view communityId,
					blockchain::AbstractProvider* blockchainProvider,
					data::ConstConfirmedTransactionPtr previousConfirmedTransaction,
					data::ConstConfirmedTransactionPtr recipientPreviousConfirmedTransaction
				);
			protected:
				const data::CommunityRoot& mCommunityRoot;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_COMMUNITY_ROOT_ROLE_H