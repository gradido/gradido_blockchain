#include "TransactionBody.h"

#include "../../SingletonManager/MemoryManager.h"
#include "lib/DataTypeConverter.h"
#include "ProtobufExceptions.h"

namespace model {
	namespace gradido {

		TransactionBody::TransactionBody()
			: mTransactionSpecific(nullptr), mType(TRANSACTION_NONE)
		{
			auto created = mProtoTransactionBody.mutable_created();
			DataTypeConverter::convertToProtoTimestampSeconds(Poco::Timestamp(), created);
		}

		void TransactionBody::setCreated(Poco::DateTime created)
		{
			auto protoCreated = mProtoTransactionBody.mutable_created();
			DataTypeConverter::convertToProtoTimestampSeconds(created.timestamp(), protoCreated);
		}

		TransactionBody::~TransactionBody()
		{
			lock("TransactionBody::~TransactionBody");
			if (mTransactionSpecific) {
				delete mTransactionSpecific;
				mTransactionSpecific = nullptr;
			}
			unlock();
		}


		std::shared_ptr<TransactionBody> TransactionBody::load(const std::string& protoMessageBin)
		{
			std::shared_ptr<TransactionBody> obj(new TransactionBody);

			if (!obj->mProtoTransactionBody.ParseFromString(protoMessageBin)) {
				return nullptr;
			}

			// check Type
			if (obj->mProtoTransactionBody.has_creation()) {
				obj->mType = TRANSACTION_CREATION;
				obj->mTransactionSpecific = new model::gradido::TransactionCreation(obj->mProtoTransactionBody.memo(), obj->mProtoTransactionBody.creation());
			}
			else if (obj->mProtoTransactionBody.has_transfer()) {
				obj->mType = TRANSACTION_TRANSFER;
				obj->mTransactionSpecific = new model::gradido::TransactionTransfer(obj->mProtoTransactionBody.memo(), obj->mProtoTransactionBody.transfer());
			}
			obj->mTransactionSpecific->prepare();
			return obj;
		}


		std::string TransactionBody::getMemo()
		{
			LOCK_RECURSIVE;
			if (mProtoTransactionBody.IsInitialized()) {
				std::string result(mProtoTransactionBody.memo());

				return result;
			}
			return "<uninitalized>";
		}

		void TransactionBody::setMemo(const std::string& memo)
		{
			LOCK_RECURSIVE;
			mProtoTransactionBody.set_memo(memo);
		}

		std::string TransactionBody::getBodyBytes()
		{
			LOCK_RECURSIVE;
			if (mProtoTransactionBody.IsInitialized()) {
				auto size = mProtoTransactionBody.ByteSizeLong();
				//auto bodyBytesSize = MemoryManager::getInstance()->getFreeMemory(mProtoCreation.ByteSizeLong());
				std::string resultString(size, 0);
				if (!mProtoTransactionBody.SerializeToString(&resultString)) {
					//addError(new Error("TransactionCreation::getBodyBytes", "error serializing string"));
					throw ProtobufSerializationException(mProtoTransactionBody);
				}

				return resultString;
			}

			return "<uninitalized>";
		}

		const char* TransactionBody::transactionTypeToString(TransactionType type)
		{
			switch (type)
			{
			case TRANSACTION_NONE: return "NONE";
			case TRANSACTION_CREATION: return "Creation";
			case TRANSACTION_TRANSFER: return "Transfer";
			case TRANSACTION_GROUP_FRIENDS_UPDATE: return "Group Friends Update";
			case TRANSACTION_REGISTER_ADDRESS: return "Register Address";
			case TRANSACTION_GLOBAL_GROUP_ADD: return "Global Group Add";
			case TRANSACTION_DEFERRED_TRANSFER: return "Deferred Transaction";
			}
			return "<unknown>";
		}



		TransactionCreation* TransactionBody::getCreationTransaction()
		{
			return dynamic_cast<TransactionCreation*>(mTransactionSpecific);
		}

		TransactionTransfer* TransactionBody::getTransferTransaction()
		{
			return dynamic_cast<TransactionTransfer*>(mTransactionSpecific);
		}

		TransactionBase* TransactionBody::getTransactionBase()
		{
			return mTransactionSpecific;
		}

	}
}
