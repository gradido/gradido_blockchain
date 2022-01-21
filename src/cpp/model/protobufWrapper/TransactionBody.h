#ifndef GRADIDO_LOGIN_SERVER_MODEL_GRADIDO_TRANSACTION_BASE_H
#define GRADIDO_LOGIN_SERVER_MODEL_GRADIDO_TRANSACTION_BASE_H

#include "gradido/TransactionBody.pb.h"
#include "TransactionTransfer.h"
#include "TransactionCreation.h"

#include "../../lib/MultithreadContainer.h"

namespace model {
	namespace gradido {

		enum TransactionType {
			TRANSACTION_NONE,
			TRANSACTION_CREATION,
			TRANSACTION_TRANSFER,
			TRANSACTION_GROUP_FRIENDS_UPDATE,
			TRANSACTION_REGISTER_ADDRESS,
			TRANSACTION_GLOBAL_GROUP_ADD,
			TRANSACTION_DEFERRED_TRANSFER
		};


		class TransactionBody : public UniLib::lib::MultithreadContainer
		{
		public:
			TransactionBody();
			virtual ~TransactionBody();

			void setCreated(Poco::DateTime created);
			inline uint32_t getCreatedSeconds() { return mProtoTransactionBody.created().seconds(); }

			static std::shared_ptr<TransactionBody> load(const std::string& protoMessageBin);

			inline TransactionType getType() { LOCK_RECURSIVE; return mType; }
			static const char* transactionTypeToString(TransactionType type);
			std::string getMemo();
			void setMemo(const std::string& memo);

			inline bool isCreation() { LOCK_RECURSIVE; return mType == TRANSACTION_CREATION; }
			inline bool isTransfer() { LOCK_RECURSIVE; return mType == TRANSACTION_TRANSFER; }
			inline bool isGroupFriendsUpdate() { LOCK_RECURSIVE; return mType == TRANSACTION_GROUP_FRIENDS_UPDATE; }
			inline bool isRegisterAddress() { LOCK_RECURSIVE; return mType == TRANSACTION_REGISTER_ADDRESS; }
			inline bool isGlobalGroupAdd() { LOCK_RECURSIVE; return mType == TRANSACTION_GLOBAL_GROUP_ADD; }
			inline bool isDeferredTransaction() { LOCK_RECURSIVE; return mType == TRANSACTION_DEFERRED_TRANSFER; }

			std::string getBodyBytes();
			const proto::gradido::TransactionBody* getBody() { return &mProtoTransactionBody; }

			TransactionCreation* getCreationTransaction();
			TransactionTransfer* getTransferTransaction();
			TransactionBase* getTransactionBase();
		protected:
			
			proto::gradido::TransactionBody mProtoTransactionBody;
			TransactionBase* mTransactionSpecific;
			TransactionType mType;
		};
	}
}

#endif //GRADIDO_LOGIN_SERVER_MODEL_GRADIDO_TRANSACTION_BASE_H
