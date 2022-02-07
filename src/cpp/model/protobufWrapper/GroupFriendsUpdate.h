#ifndef __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_GROUP_FRIENDS_UPDATE_H
#define __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_GROUP_FRIENDS_UPDATE_H

#include "TransactionBase.h"
#include "gradido/GroupFriendsUpdate.pb.h"

namespace model {
	namespace gradido {
		class GroupFriendsUpdate : public TransactionBase
		{
		public:
			GroupFriendsUpdate(const proto::gradido::GroupFriendsUpdate& protoGroupFriendsUpdate);
			~GroupFriendsUpdate();

			bool validate(TransactionValidationLevel level = TRANSACTION_VALIDATION_SINGLE, IGradidoBlockchain* blockchain = nullptr) const;

			inline bool isColorFusion() const { return mProtoGroupFriendsUpdate.color_fusion(); }
		protected:
			const proto::gradido::GroupFriendsUpdate& mProtoGroupFriendsUpdate;
		};
	}
}



#endif //__GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_GROUP_FRIENDS_UPDATE_H