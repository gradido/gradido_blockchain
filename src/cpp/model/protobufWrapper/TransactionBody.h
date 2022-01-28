#ifndef GRADIDO_LOGIN_SERVER_MODEL_GRADIDO_TRANSACTION_BODY_H
#define GRADIDO_LOGIN_SERVER_MODEL_GRADIDO_TRANSACTION_BODY_H

#include "gradido/TransactionBody.pb.h"

#include "DeferredTransfer.h"
#include "GlobalGroupAdd.h"
#include "GroupFriendsUpdate.h"
#include "RegisterAddress.h"
#include "TransactionCreation.h"
#include "TransactionTransfer.h"

#include "Poco/DateTime.h"

#include "../../lib/MultithreadContainer.h"

namespace model {
	namespace gradido {

		class TransactionBody : public UniLib::lib::MultithreadContainer
		{
		public:
			virtual ~TransactionBody();

			void setCreated(Poco::DateTime created);
			inline uint32_t getCreatedSeconds() const { return mProtoTransactionBody.created().seconds(); }

			static TransactionBody* load(const std::string& protoMessageBin);

			inline TransactionType getTransactionType() const { return mTransactionType; }
			inline proto::gradido::TransactionBody_CrossGroupType getCrossGroupType() const { return mProtoTransactionBody.type(); }
			inline uint64_t getVersionNumber() const { return mProtoTransactionBody.version_number(); }
			inline const std::string& getOtherGroup() const { return mProtoTransactionBody.other_group(); }
			static const char* transactionTypeToString(TransactionType type);
			std::string getMemo() const;
			void setMemo(const std::string& memo);

			inline bool isDeferredTransfer() const { return mTransactionType == TRANSACTION_DEFERRED_TRANSFER; }
			inline bool isGlobalGroupAdd() const { return mTransactionType == TRANSACTION_GLOBAL_GROUP_ADD; }
			inline bool isGroupFriendsUpdate() const { return mTransactionType == TRANSACTION_GROUP_FRIENDS_UPDATE; }
			inline bool isRegisterAddress() const { return mTransactionType == TRANSACTION_REGISTER_ADDRESS; }
			inline bool isCreation() const { return mTransactionType == TRANSACTION_CREATION; }
			inline bool isTransfer() const  { return mTransactionType == TRANSACTION_TRANSFER; }		

			bool validate(TransactionValidationLevel level = TRANSACTION_VALIDATION_SINGLE, IGradidoBlockchain* blockchain = nullptr) const;

			std::string getBodyBytes() const;
			const proto::gradido::TransactionBody* getBody() const { return &mProtoTransactionBody; }

			const DeferredTransfer* getDeferredTransfer() const;
			const GlobalGroupAdd* getGlobalGroupAdd() const;
			const GroupFriendsUpdate* getGroupFriendsUpdate() const;
			const RegisterAddress* getRegisterAddress() const;
			const TransactionCreation* getCreationTransaction() const;
			const TransactionTransfer* getTransferTransaction() const;

			const TransactionBase* getTransactionBase() const;
		protected:
			TransactionBody();
			
			proto::gradido::TransactionBody mProtoTransactionBody;
			TransactionBase* mTransactionSpecific;
			TransactionType mTransactionType;
		};
	}
}

#endif //GRADIDO_LOGIN_SERVER_MODEL_GRADIDO_TRANSACTION_BODY_H
