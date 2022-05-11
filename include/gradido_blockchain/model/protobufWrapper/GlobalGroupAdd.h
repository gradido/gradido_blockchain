#ifndef __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_GLOBAL_GROUP_ADD_H
#define __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_GLOBAL_GROUP_ADD_H

#include "TransactionBase.h"
#include "proto/gradido/GlobalGroupAdd.pb.h"

#define GROUP_REGISTER_GROUP_ALIAS "GroupRegister"

namespace model {
	namespace gradido {

		class GRADIDOBLOCKCHAIN_EXPORT GlobalGroupAdd : public TransactionBase
		{
		public:
			GlobalGroupAdd(const proto::gradido::GlobalGroupAdd& protoGlobalGroupAdd);
			~GlobalGroupAdd();

			int prepare();
			bool validate(
				TransactionValidationLevel level = TRANSACTION_VALIDATION_SINGLE,
				IGradidoBlockchain* blockchain = nullptr,
				const GradidoBlock* parentGradidoBlock = nullptr
			) const;
			

			bool isBelongToUs(const TransactionBase* pairingTransaction) const;

			std::vector<MemoryBin*> getInvolvedAddresses() const;

			inline const std::string& getGroupName() const { return mProtoGlobalGroupAdd.group_name(); }
			inline const std::string& getGroupAlias() const { return mProtoGlobalGroupAdd.group_alias(); }
			uint32_t getCoinColor() const;

		protected:
			const proto::gradido::GlobalGroupAdd& mProtoGlobalGroupAdd;

		};
	}
}



#endif // __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_GLOBAL_GROUP_ADD_H