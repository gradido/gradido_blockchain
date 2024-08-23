#ifndef __GRADIDO_BLOCKCHAIN_DATA_COMMUNITY_FRIENDS_UPDATE_H
#define __GRADIDO_BLOCKCHAIN_DATA_COMMUNITY_FRIENDS_UPDATE_H

#include "gradido_blockchain/export.h"

namespace gradido {
	namespace data {
		class GRADIDOBLOCKCHAIN_EXPORT CommunityFriendsUpdate
		{
		public:
			CommunityFriendsUpdate(bool colorFusion) : mColorFusion(colorFusion) {}

			inline bool operator==(const CommunityFriendsUpdate& other) const {
				return mColorFusion == other.mColorFusion;
			}
			inline bool getColorFusion() const { return mColorFusion; }
		private: 
			bool mColorFusion;
		};
	}
}
#endif //__GRADIDO_BLOCKCHAIN_DATA_COMMUNITY_FRIENDS_UPDATE_H