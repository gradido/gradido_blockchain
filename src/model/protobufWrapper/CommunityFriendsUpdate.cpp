#include "gradido_blockchain/model/protobufWrapper/CommunityFriendsUpdate.h"

namespace model {
	namespace gradido {
		CommunityFriendsUpdate::CommunityFriendsUpdate(const proto::gradido::CommunityFriendsUpdate& protoGroupFriendsUpdate)
			: mProtoGroupFriendsUpdate(protoGroupFriendsUpdate)
		{

		}

		CommunityFriendsUpdate::~CommunityFriendsUpdate()
		{

		}

		bool CommunityFriendsUpdate::validate(
			TransactionValidationLevel level/* = TRANSACTION_VALIDATION_SINGLE*/,
			IGradidoBlockchain* blockchain/* = nullptr*/,
			const ConfirmedTransaction* parentConfirmedTransaction/* = nullptr*/
		) const
		{
			return true;
		}

		std::vector<MemoryBin*> CommunityFriendsUpdate::getInvolvedAddresses() const
		{
			return {};
		}
		bool CommunityFriendsUpdate::isInvolved(const std::string pubkeyString) const
		{
			return false;
		}

		bool CommunityFriendsUpdate::isBelongToUs(const TransactionBase* pairingTransaction) const
		{
			auto pair = dynamic_cast<const CommunityFriendsUpdate*>(pairingTransaction);
			return isColorFusion() == pair->isColorFusion();
		}
		bool CommunityFriendsUpdate::isColorFusion() const
		{
			return mProtoGroupFriendsUpdate.color_fusion();
		}

	}
}
