#include "TransactionBody.h"

#include "../../SingletonManager/MemoryManager.h"
#include "lib/DataTypeConverter.h"
#include "ProtobufExceptions.h"
#include "TransactionValidationExceptions.h"

namespace model {
	namespace gradido {

		TransactionBody::TransactionBody()
			: mTransactionSpecific(nullptr), mTransactionType(TRANSACTION_NONE)
		{
			auto created = mProtoTransactionBody.mutable_created();
			DataTypeConverter::convertToProtoTimestampSeconds(Poco::Timestamp(), created);
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

		void TransactionBody::setCreated(Poco::DateTime created)
		{
			auto protoCreated = mProtoTransactionBody.mutable_created();
			DataTypeConverter::convertToProtoTimestampSeconds(created.timestamp(), protoCreated);
		}

		TransactionBody* TransactionBody::load(const std::string& protoMessageBin)
		{
			auto obj = new TransactionBody;

			if (!obj->mProtoTransactionBody.ParseFromString(protoMessageBin)) {
				delete obj;
				throw ProtobufParseException(protoMessageBin);
			}

			// check Type
			if (obj->mProtoTransactionBody.has_creation()) {
				obj->mTransactionType = TRANSACTION_CREATION;
				obj->mTransactionSpecific = new model::gradido::TransactionCreation(obj->mProtoTransactionBody.creation());
			}
			else if (obj->mProtoTransactionBody.has_transfer()) {
				obj->mTransactionType = TRANSACTION_TRANSFER;
				obj->mTransactionSpecific = new model::gradido::TransactionTransfer(obj->mProtoTransactionBody.transfer());
			}
			else if (obj->mProtoTransactionBody.has_global_group_add()) {
				obj->mTransactionType = TRANSACTION_GLOBAL_GROUP_ADD;
				obj->mTransactionSpecific = new model::gradido::GlobalGroupAdd(obj->mProtoTransactionBody.global_group_add());
			}
			else if (obj->mProtoTransactionBody.has_group_friends_update()) {
				obj->mTransactionType = TRANSACTION_GROUP_FRIENDS_UPDATE;
				obj->mTransactionSpecific = new model::gradido::GroupFriendsUpdate(obj->mProtoTransactionBody.group_friends_update());
			}
			else if (obj->mProtoTransactionBody.has_register_address()) {
				obj->mTransactionType = TRANSACTION_REGISTER_ADDRESS;
				obj->mTransactionSpecific = new model::gradido::RegisterAddress(obj->mProtoTransactionBody.register_address());
			}
			else if (obj->mProtoTransactionBody.has_deferred_transfer()) {
				obj->mTransactionType = TRANSACTION_DEFERRED_TRANSFER;
				obj->mTransactionSpecific = new model::gradido::DeferredTransfer(obj->mProtoTransactionBody.deferred_transfer());
			}
			obj->mTransactionSpecific->prepare();
			return obj;
		}


		std::string TransactionBody::getMemo() const
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

		std::string TransactionBody::getBodyBytes() const
		{
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

		const DeferredTransfer* TransactionBody::getDeferredTransfer() const
		{
			return dynamic_cast<DeferredTransfer*>(mTransactionSpecific);
		}

		const GlobalGroupAdd* TransactionBody::getGlobalGroupAdd() const
		{
			return dynamic_cast<GlobalGroupAdd*>(mTransactionSpecific);
		}

		const GroupFriendsUpdate* TransactionBody::getGroupFriendsUpdate() const
		{
			return dynamic_cast<GroupFriendsUpdate*>(mTransactionSpecific);
		}

		const RegisterAddress* TransactionBody::getRegisterAddress() const
		{
			return dynamic_cast<RegisterAddress*>(mTransactionSpecific);
		}

		const TransactionCreation* TransactionBody::getCreationTransaction() const
		{
			return dynamic_cast<TransactionCreation*>(mTransactionSpecific);
		}

		const TransactionTransfer* TransactionBody::getTransferTransaction() const
		{
			return dynamic_cast<TransactionTransfer*>(mTransactionSpecific);
		}

		const TransactionBase* TransactionBody::getTransactionBase() const
		{
			return mTransactionSpecific;
		}
		
		bool TransactionBody::validate(TransactionValidationLevel level/* = TRANSACTION_VALIDATION_SINGLE*/, IGradidoBlockchain* blockchain/* = nullptr*/) const
		{
			if ((level & TRANSACTION_VALIDATION_SINGLE) == TRANSACTION_VALIDATION_SINGLE) {
				if (getVersionNumber() != 2) {
					throw TransactionValidationInvalidInputException("wrong version", "version_number", "uint64")
						.setTransactionBody(this);
				}
				if (getMemo().size() < 5 || getMemo().size() > 350) {
					throw TransactionValidationInvalidInputException("not in expected range [5;350]", "memo", "string")
						.setTransactionBody(this);
				}
				if (!TransactionBase::isValidGroupAlias(getOtherGroup())) {
					throw TransactionValidationInvalidInputException("invalid character, only ascii", "other_group", "string")
						.setTransactionBody(this);
				}
				if (isDeferredTransfer()) {
					auto deferredTransfer = getDeferredTransfer();
					auto timeout = deferredTransfer->getTimeoutAsPocoTimestamp();
					if (getCreatedSeconds() >= timeout.epochTime()) {
						throw TransactionValidationInvalidInputException("already reached", "timeout", "Timestamp")
							.setTransactionBody(this);
					}
				}
			}
			try {
				mTransactionSpecific->validate(level, blockchain);
			}
			catch (TransactionValidationException& ex) {
				ex.setTransactionBody(this);
				throw;
			}

			return true;

		}

	}
}
