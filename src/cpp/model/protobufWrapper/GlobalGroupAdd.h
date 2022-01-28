#ifndef __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_GLOBAL_GROUP_ADD_H
#define __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_GLOBAL_GROUP_ADD_H

#include "TransactionBase.h"
#include "gradido/GlobalGroupAdd.pb.h"


namespace model {
	namespace gradido {

		class GlobalGroupAdd : public TransactionBase
		{
		public:
			GlobalGroupAdd(const proto::gradido::GlobalGroupAdd& protoGlobalGroupAdd);
			~GlobalGroupAdd();

			bool validate(TransactionValidationLevel level = TRANSACTION_VALIDATION_SINGLE, IGradidoBlockchain* blockchain = nullptr) const;

			inline const std::string& getGroupName() const { return mProtoGlobalGroupAdd.group_name(); }
			inline const std::string& getGroupAlias() const { return mProtoGlobalGroupAdd.group_alias(); }
			inline uint32_t getCoinColor() const { return mProtoGlobalGroupAdd.native_coin_color(); }

		protected:
			const proto::gradido::GlobalGroupAdd& mProtoGlobalGroupAdd;

		};
	}
}



#endif // __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_GLOBAL_GROUP_ADD_H