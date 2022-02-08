#include "gradido_blockchain/model/protobufWrapper/GroupFriendsUpdate.h"

namespace model {
	namespace gradido {
		GroupFriendsUpdate::GroupFriendsUpdate(const proto::gradido::GroupFriendsUpdate& protoGroupFriendsUpdate)
			: mProtoGroupFriendsUpdate(protoGroupFriendsUpdate)
		{

		}

		GroupFriendsUpdate::~GroupFriendsUpdate()
		{

		}

		bool GroupFriendsUpdate::validate(TransactionValidationLevel level/* = TRANSACTION_VALIDATION_SINGLE*/, IGradidoBlockchain* blockchain/* = nullptr*/) const
		{
			return true;
		}

	}
}