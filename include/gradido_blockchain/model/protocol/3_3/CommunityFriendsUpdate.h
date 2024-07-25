#ifndef __GRADIDO_BLOCKCHAIN_MODEL_PROTOCOL_3_3_COMMUNITY_FRIENDS_UPDATE_H
#define __GRADIDO_BLOCKCHAIN_MODEL_PROTOCOL_3_3_COMMUNITY_FRIENDS_UPDATE_H

#include "TransactionBase.h"

namespace model {
	namespace protocol {
		namespace _3_3 {
			class GRADIDOBLOCKCHAIN_EXPORT CommunityFriendsUpdate : public TransactionBase
			{
			public:
				CommunityFriendsUpdate(const proto::gradido::CommunityFriendsUpdate& protoGroupFriendsUpdate);
				~CommunityFriendsUpdate();

				bool validate(
					TransactionValidationLevel level = TRANSACTION_VALIDATION_SINGLE,
					IGradidoBlockchain* blockchain = nullptr,
					const ConfirmedTransaction* parentConfirmedTransaction = nullptr
					) const;
				std::vector<std::string_view> getInvolvedAddresses() const;
				bool isInvolved(const std::string pubkeyString) const;
				bool isBelongToUs(const TransactionBase* pairingTransaction) const;

				bool isColorFusion() const;
			protected:
				const proto::gradido::CommunityFriendsUpdate& mProtoGroupFriendsUpdate;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_MODEL_PROTOCOL_3_3_COMMUNITY_FRIENDS_UPDATE_H
