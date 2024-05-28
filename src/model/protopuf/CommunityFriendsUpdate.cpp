#include "gradido_blockchain/model/protopuf/CommunityFriendsUpdate.h"

namespace model {
	namespace protopuf {
		CommunityFriendsUpdate::CommunityFriendsUpdate(const CommunityFriendsUpdateMessage& data)
			: mColorFusion(data["color_fusion"_f].value())
		{

		}

		CommunityFriendsUpdate::CommunityFriendsUpdate(bool colorFusion)
			: mColorFusion(colorFusion)
		{

		}
	}
}