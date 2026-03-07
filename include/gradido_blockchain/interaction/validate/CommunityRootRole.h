#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_COMMUNITY_ROOT_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_COMMUNITY_ROOT_ROLE_H

#include "AbstractRole.h"
#include "Error.h"
#include "gradido_blockchain/data/compact/CommunityRootTx.h"
#include "Options.h"

#include <memory>

namespace gradido {	
	namespace data::compact {
		struct ConfirmedGradidoTx;
	}
	namespace blockchain {
		class Abstract;
	}
	namespace interaction::validate {

		GRADIDOBLOCKCHAIN_EXPORT Error validateCommunityRoot(
			const data::compact::ConfirmedGradidoTx& tx, 
			const std::shared_ptr<const blockchain::Abstract>& blockchain,
			Options options
		);

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

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_COMMUNITY_ROOT_ROLE_H