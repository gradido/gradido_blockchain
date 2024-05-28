#ifndef __GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_COMMUNITY_FRIENDS_UPDATE_H
#define __GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_COMMUNITY_FRIENDS_UPDATE_H

#include <bit>
#include "protopuf/message.h"
#include "gradido_blockchain/export.h"

using namespace pp;

namespace model {
	namespace protopuf {

		using CommunityFriendsUpdateMessage = message<
			bool_field<"color_fusion", 1>
		>;

		class GRADIDOBLOCKCHAIN_EXPORT CommunityFriendsUpdate
		{
		public:
			CommunityFriendsUpdate(const CommunityFriendsUpdateMessage& data);
			CommunityFriendsUpdate(bool colorFusion);
			~CommunityFriendsUpdate() {}

			inline bool isColorFusion() const { return mColorFusion; }

		protected:
			bool mColorFusion;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_COMMUNITY_FRIENDS_UPDATE_H