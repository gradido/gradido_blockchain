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

		bool GroupFriendsUpdate::validate(
			TransactionValidationLevel level/* = TRANSACTION_VALIDATION_SINGLE*/,
			IGradidoBlockchain* blockchain/* = nullptr*/,
			const GradidoBlock* parentGradidoBlock/* = nullptr*/
		) const
		{
			return true;
		}

		std::vector<MemoryBin*> GroupFriendsUpdate::getInvolvedAddresses() const
		{
			return {};
		}

		bool GroupFriendsUpdate::isBelongToUs(const TransactionBase* pairingTransaction) const
		{
			auto pair = dynamic_cast<const GroupFriendsUpdate*>(pairingTransaction);
			return isColorFusion() == pair->isColorFusion();
		}

	}
}