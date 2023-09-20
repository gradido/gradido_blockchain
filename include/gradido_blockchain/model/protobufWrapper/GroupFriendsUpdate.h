#ifndef __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_GROUP_FRIENDS_UPDATE_H
#define __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_GROUP_FRIENDS_UPDATE_H

#include "TransactionBase.h"
#include "proto/gradido/group_friends_update.pb.h"

namespace model {
	namespace gradido {
		class GRADIDOBLOCKCHAIN_EXPORT GroupFriendsUpdate : public TransactionBase
		{
		public:
			GroupFriendsUpdate(const proto::gradido::GroupFriendsUpdate& protoGroupFriendsUpdate);
			~GroupFriendsUpdate();

			bool validate(
				TransactionValidationLevel level = TRANSACTION_VALIDATION_SINGLE,
				IGradidoBlockchain* blockchain = nullptr,
				const ConfirmedTransaction* parentGradidoBlock = nullptr
				) const;
			std::vector<MemoryBin*> getInvolvedAddresses() const;
			bool isInvolved(const std::string pubkeyString) const;
			bool isBelongToUs(const TransactionBase* pairingTransaction) const;

			bool isColorFusion() const;
		protected:
			const proto::gradido::GroupFriendsUpdate& mProtoGroupFriendsUpdate;
		};
	}
}



#endif //__GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_GROUP_FRIENDS_UPDATE_H
