#ifndef __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_COMMUNITY_ROOT_H
#define __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_COMMUNITY_ROOT_H

#include "gradido_blockchain/model/protobufWrapper/TransactionBase.h"
#include "proto/gradido/community_root.pb.h"

namespace model {
	namespace gradido {
		class GRADIDOBLOCKCHAIN_EXPORT CommunityRoot : public TransactionBase
		{
		public:
			CommunityRoot(const proto::gradido::CommunityRoot& protoCommunityRoot);
			~CommunityRoot();

			int prepare();
			bool validate(
				TransactionValidationLevel level = TRANSACTION_VALIDATION_SINGLE,
				IGradidoBlockchain* blockchain = nullptr,
				const ConfirmedTransaction* parentConfirmedTransaction = nullptr
			) const;
			std::vector<std::string_view> getInvolvedAddresses() const;
			bool isInvolved(const std::string& pubkeyString) const;
			inline bool isBelongToUs(const TransactionBase* pairingTransaction) const { return false; }

			const std::string& getPubkey() const;
			const std::string& getGMWPubkey() const;
			const std::string& getAUFPubkey() const;

		protected:
			const proto::gradido::CommunityRoot& mProtoCommunityRoot;
		};

	}
}

#endif //__GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_COMMUNITY_ROOT_H