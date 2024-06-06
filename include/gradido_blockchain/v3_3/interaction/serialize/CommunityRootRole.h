#ifndef __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_SERIALIZE_COMMUNITY_ROOT_ROLE_H
#define __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_SERIALIZE_COMMUNITY_ROOT_ROLE_H

#include "gradido_blockchain/v3_3/data/Protocol.h"
#include "gradido_blockchain/v3_3/data/Protopuf.h"
#include "AbstractRole.h"

#include <bit>
#include "protopuf/message.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace serialize {

				// community_root.proto
				using CommunityRootMessage = pp::message<
					pp::bytes_field<"pubkey", 1>,
					pp::bytes_field<"gmw_pubkey", 2>,
					pp::bytes_field<"auf_pubkey", 3>
				>;

				class CommunityRootRole : public AbstractRole
				{
				public:
					CommunityRootRole(const data::CommunityRoot& communityRoot) : mCommunityRoot(communityRoot) {}
					~CommunityRootRole() {};

					RUM_IMPLEMENTATION
					CommunityRootMessage getMessage() const;
					virtual size_t calculateSerializedSize() const;

				protected:
					const data::CommunityRoot& mCommunityRoot;
				};
			}
		}
	}
}


#endif // __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_SERIALIZE_COMMUNITY_ROOT_ROLE_H