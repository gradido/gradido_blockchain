#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_ADVANCED_BLOCKCHAIN_FILTER_COMMUNITY_ROOT_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_ADVANCED_BLOCKCHAIN_FILTER_COMMUNITY_ROOT_ROLE_H

#include "AbstractTransactionRole.h"

namespace gradido {

	namespace interaction {
		namespace advancedBlockchainFilter {
			class CommunityRootTransactionRole : public AbstractTransactionRole
			{
			public:
				using AbstractTransactionRole::AbstractTransactionRole;

				memory::ConstBlockPtr getSenderPublicKey() const { return nullptr; };
				memory::ConstBlockPtr getRecipientPublicKey() const { return nullptr; };

			protected:
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_ADVANCED_BLOCKCHAIN_FILTER_COMMUNITY_ROOT_ROLE_H