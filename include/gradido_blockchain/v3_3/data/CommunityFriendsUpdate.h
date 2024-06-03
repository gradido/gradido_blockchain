#ifndef __GRADIDO_BLOCKCHAIN_v3_3_DATA_COMMUNITY_FRIENDS_UPDATE_H
#define __GRADIDO_BLOCKCHAIN_v3_3_DATA_COMMUNITY_FRIENDS_UPDATE_H

#include "gradido_blockchain/export.h"

namespace gradido {
	namespace v3_3 {
		namespace data {
			struct GRADIDOBLOCKCHAIN_EXPORT CommunityFriendsUpdate
			{
				CommunityFriendsUpdate(bool _colorFusion) : colorFusion(_colorFusion)  {}
				~CommunityFriendsUpdate();

				bool colorFusion;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_v3_3_DATA_COMMUNITY_FRIENDS_UPDATE_H