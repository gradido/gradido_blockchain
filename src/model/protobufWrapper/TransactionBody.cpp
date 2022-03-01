#include "gradido_blockchain/model/protobufWrapper/TransactionBody.h"

#include "gradido_blockchain/MemoryManager.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"
#include "gradido_blockchain/model/IGradidoBlockchain.h"
#include "gradido_blockchain/model/protobufWrapper/ProtobufExceptions.h"
#include "gradido_blockchain/model/protobufWrapper/TransactionValidationExceptions.h"

namespace model {
	namespace gradido {

		TransactionBody::TransactionBody()
			: mTransactionSpecific(nullptr), mTransactionType(TRANSACTION_NONE)
		{
			auto created = mProtoTransactionBody.mutable_created();
			DataTypeConverter::convertToProtoTimestampSeconds(Poco::Timestamp(), created);
			mProtoTransactionBody.set_type(proto::gradido::TransactionBody_CrossGroupType_LOCAL);
			mProtoTransactionBody.set_version_number(GRADIDO_PROTOCOL_VERSION);
		}

		TransactionBody::~TransactionBody()
		{
			LOCK_RECURSIVE;
			lock("TransactionBody::~TransactionBody");
			if (mTransactionSpecific) {
				delete mTransactionSpecific;
				mTransactionSpecific = nullptr;
			}
			unlock();
		}

		void TransactionBody::setCreated(Poco::DateTime created)
		{
			LOCK_RECURSIVE;
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
			obj->initSpecificTransaction();
			return obj;
		}

		void TransactionBody::upgradeToDeferredTransaction(Poco::Timestamp timeout)
		{
			LOCK_RECURSIVE;
			assert(mProtoTransactionBody.has_transfer());
			delete mTransactionSpecific;
			auto transfer = mProtoTransactionBody.mutable_transfer();
			auto deferredTransfer = mProtoTransactionBody.mutable_deferred_transfer();
			// move transfer object to deferred transfer
			deferredTransfer->mutable_transfer()->Swap(transfer);
			assert(!mProtoTransactionBody.has_transfer());
			DataTypeConverter::convertToProtoTimestamp(timeout, deferredTransfer->mutable_timeout());
			initSpecificTransaction();
		}

		TransactionBody* TransactionBody::createGlobalGroupAdd(const std::string& groupName, const std::string& groupAlias, uint32_t nativeCoinColor)
		{
			auto obj = new TransactionBody;
			auto globalGroupAdd = obj->mProtoTransactionBody.mutable_global_group_add();
			globalGroupAdd->set_group_name(groupName);
			globalGroupAdd->set_group_name(groupAlias);
			globalGroupAdd->set_native_coin_color(nativeCoinColor);
			obj->initSpecificTransaction();
			return obj;
		}

		TransactionBody* TransactionBody::createGroupFriendsUpdate(bool colorFusion)
		{
			auto obj = new TransactionBody;
			auto groupFriendsUpdate = obj->mProtoTransactionBody.mutable_group_friends_update();
			groupFriendsUpdate->set_color_fusion(colorFusion);
			obj->initSpecificTransaction();
			return obj;
		}

		TransactionBody* TransactionBody::createRegisterAddress(
			const MemoryBin* userPubkey,
			proto::gradido::RegisterAddress_AddressType type,
			const MemoryBin* nameHash,
			const MemoryBin* subaccountPubkey
		)
		{
			auto obj = new TransactionBody;
			auto registerAddress = obj->mProtoTransactionBody.mutable_register_address();
			if (userPubkey) {
				registerAddress->set_allocated_user_pubkey(userPubkey->copyAsString().release());
			}
			registerAddress->set_address_type(type);
			if (nameHash) {
				registerAddress->set_allocated_name_hash(nameHash->copyAsString().release());
			}
			if (subaccountPubkey) {
				registerAddress->set_allocated_subaccount_pubkey(subaccountPubkey->copyAsString().release());
			}
			obj->initSpecificTransaction();
			return obj;
		}

		TransactionBody* TransactionBody::createTransactionCreation(std::unique_ptr<proto::gradido::TransferAmount> transferAmount, Poco::DateTime targetDate)
		{
			auto obj = new TransactionBody;
			auto creation = obj->mProtoTransactionBody.mutable_creation();
			creation->set_allocated_recipient(transferAmount.release());
			auto protoTargetDate = creation->mutable_target_date();
			DataTypeConverter::convertToProtoTimestampSeconds(targetDate.timestamp(), protoTargetDate);
			obj->initSpecificTransaction();
			return obj;
		}

		TransactionBody* TransactionBody::createTransactionTransfer(std::unique_ptr<proto::gradido::TransferAmount> transferAmount, const MemoryBin* recipientPubkey)
		{
			auto obj = new TransactionBody;
			auto transfer = obj->mProtoTransactionBody.mutable_transfer();
			transfer->set_allocated_sender(transferAmount.release());
			transfer->set_allocated_recipient(recipientPubkey->copyAsString().release());
			obj->initSpecificTransaction();
			return obj;
		}

		void TransactionBody::updateToOutbound(const std::string& otherGroup)
		{
			LOCK_RECURSIVE;
			mProtoTransactionBody.set_other_group(otherGroup);
			mProtoTransactionBody.set_type(proto::gradido::TransactionBody_CrossGroupType_OUTBOUND);
		}

		void TransactionBody::updateToInbound(const std::string& otherGroup)
		{
			LOCK_RECURSIVE;
			mProtoTransactionBody.set_other_group(otherGroup);
			mProtoTransactionBody.set_type(proto::gradido::TransactionBody_CrossGroupType_INBOUND);
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

		uint32_t TransactionBody::getCoinColor(const IGradidoBlockchain* blockchain) const
		{			
			auto color = mTransactionSpecific->getCoinColor();
			if (!color) {
				color = blockchain->getGroupDefaultCoinColor();
			}
			return color;
		}

		void TransactionBody::setMemo(const std::string& memo)
		{
			LOCK_RECURSIVE;
			mProtoTransactionBody.set_memo(memo);
		}

		std::unique_ptr<std::string> TransactionBody::getBodyBytes() const
		{
			LOCK_RECURSIVE;
			assert(mProtoTransactionBody.IsInitialized());
			
			auto size = mProtoTransactionBody.ByteSizeLong();
			//auto bodyBytesSize = MemoryManager::getInstance()->getFreeMemory(mProtoCreation.ByteSizeLong());
			std::string* resultString(new std::string(size, 0));
			if (!mProtoTransactionBody.SerializeToString(resultString)) {
				//addError(new Error("TransactionCreation::getBodyBytes", "error serializing string"));
				throw ProtobufSerializationException(mProtoTransactionBody);
			}

			std::unique_ptr<std::string> result;
			result.reset(resultString);
			return result;
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
		
		bool TransactionBody::validate(
			TransactionValidationLevel level/* = TRANSACTION_VALIDATION_SINGLE*/,
			IGradidoBlockchain* blockchain/* = nullptr*/,
			const GradidoBlock* parentGradidoBlock/* = nullptr*/) const
		{
			LOCK_RECURSIVE;
			if ((level & TRANSACTION_VALIDATION_SINGLE) == TRANSACTION_VALIDATION_SINGLE) {
				if (getVersionNumber() != GRADIDO_PROTOCOL_VERSION) {
					TransactionValidationInvalidInputException exception("wrong version", "version_number", "uint64");
					exception.setTransactionBody(this);
					throw exception;
				}
				// memo is only mandatory for transfer and creation transactions
				if (isDeferredTransfer() || isTransfer() || isCreation()) {
					if (getMemo().size() < 5 || getMemo().size() > 350) {
						TransactionValidationInvalidInputException exception("not in expected range [5;350]", "memo", "string");
						exception.setTransactionBody(this);
						throw exception;
					}
				}
				auto otherGroup = getOtherGroup();
				if (otherGroup.size() && !TransactionBase::isValidGroupAlias(getOtherGroup())) {
					TransactionValidationInvalidInputException exception("invalid character, only ascii", "other_group", "string");
					exception.setTransactionBody(this);
					throw exception;
				}
				if (isDeferredTransfer()) {
					auto deferredTransfer = getDeferredTransfer();
					auto timeout = deferredTransfer->getTimeoutAsPocoTimestamp();
					if (getCreatedSeconds() >= timeout.epochTime()) {
						TransactionValidationInvalidInputException exception("already reached", "timeout", "Timestamp");
						exception.setTransactionBody(this);
						throw exception;
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

		void TransactionBody::initSpecificTransaction()
		{
			// check Type
			if (mProtoTransactionBody.has_creation()) {
				mTransactionType = TRANSACTION_CREATION;
				mTransactionSpecific = new model::gradido::TransactionCreation(mProtoTransactionBody.creation());
			}
			else if (mProtoTransactionBody.has_transfer()) {
				mTransactionType = TRANSACTION_TRANSFER;
				mTransactionSpecific = new model::gradido::TransactionTransfer(mProtoTransactionBody.transfer());
			}
			else if (mProtoTransactionBody.has_global_group_add()) {
				mTransactionType = TRANSACTION_GLOBAL_GROUP_ADD;
				mTransactionSpecific = new model::gradido::GlobalGroupAdd(mProtoTransactionBody.global_group_add());
			}
			else if (mProtoTransactionBody.has_group_friends_update()) {
				mTransactionType = TRANSACTION_GROUP_FRIENDS_UPDATE;
				mTransactionSpecific = new model::gradido::GroupFriendsUpdate(mProtoTransactionBody.group_friends_update());
			}
			else if (mProtoTransactionBody.has_register_address()) {
				mTransactionType = TRANSACTION_REGISTER_ADDRESS;
				mTransactionSpecific = new model::gradido::RegisterAddress(mProtoTransactionBody.register_address());
			}
			else if (mProtoTransactionBody.has_deferred_transfer()) {
				mTransactionType = TRANSACTION_DEFERRED_TRANSFER;
				mTransactionSpecific = new model::gradido::DeferredTransfer(mProtoTransactionBody.deferred_transfer());
			}
			mTransactionSpecific->prepare();
		}

		

	}
}
