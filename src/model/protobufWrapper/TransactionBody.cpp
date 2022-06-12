#include "gradido_blockchain/model/protobufWrapper/TransactionBody.h"

#include "gradido_blockchain/MemoryManager.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"
#include "gradido_blockchain/model/IGradidoBlockchain.h"
#include "gradido_blockchain/model/protobufWrapper/ProtobufExceptions.h"
#include "gradido_blockchain/model/protobufWrapper/TransactionValidationExceptions.h"

namespace model {
	namespace gradido {

		TransactionBody::TransactionBody()
			: TransactionBody(ProtobufArenaMemory::create())
		{
		}

		TransactionBody::TransactionBody(std::shared_ptr<ProtobufArenaMemory> arenaMemory)
			: mProtoTransactionBody(google::protobuf::Arena::CreateMessage<proto::gradido::TransactionBody>(*arenaMemory)),
		      mTransactionSpecific(nullptr), mTransactionType(TRANSACTION_NONE), mProtobufArenaMemory(arenaMemory)
		{
			if (arenaMemory->getUsedSpace() > 7168) {
				int zahl = 1;
			}
			auto created = mProtoTransactionBody->mutable_created();
			DataTypeConverter::convertToProtoTimestampSeconds(Poco::Timestamp(), created);
			mProtoTransactionBody->set_type(proto::gradido::TransactionBody_CrossGroupType_LOCAL);
			mProtoTransactionBody->set_version_number(GRADIDO_PROTOCOL_VERSION);
		}

		TransactionBody::~TransactionBody()
		{
			LOCK_RECURSIVE;
			lock("TransactionBody::~TransactionBody");
			mProtoTransactionBody = nullptr;
			if (mTransactionSpecific) {
				delete mTransactionSpecific;
				mTransactionSpecific = nullptr;
			}
			unlock();
			printf("[~TransactionBody]\n");
		}

		void TransactionBody::setCreated(Poco::DateTime created)
		{
			LOCK_RECURSIVE;
			auto protoCreated = mProtoTransactionBody->mutable_created();
			DataTypeConverter::convertToProtoTimestampSeconds(created.timestamp(), protoCreated);
		}

		uint32_t TransactionBody::getCreatedSeconds() const
		{
			return mProtoTransactionBody->created().seconds();
		}

		TransactionBody* TransactionBody::load(const std::string& protoMessageBin, std::shared_ptr<ProtobufArenaMemory> arenaMemory)
		{
			auto obj = new TransactionBody(arenaMemory);

			if (!obj->mProtoTransactionBody->ParseFromString(protoMessageBin)) {
				delete obj;
				throw ProtobufParseException(protoMessageBin);
			}
			obj->initSpecificTransaction();
			return obj;
		}

		void TransactionBody::upgradeToDeferredTransaction(Poco::Timestamp timeout)
		{
			LOCK_RECURSIVE;
			assert(mProtoTransactionBody->has_transfer());
			delete mTransactionSpecific;
			auto transfer = mProtoTransactionBody->mutable_transfer();
			auto deferredTransfer = mProtoTransactionBody->mutable_deferred_transfer();
			// move transfer object to deferred transfer
			deferredTransfer->mutable_transfer()->Swap(transfer);
			assert(!mProtoTransactionBody->has_transfer());
			DataTypeConverter::convertToProtoTimestamp(timeout, deferredTransfer->mutable_timeout());
			initSpecificTransaction();
		}


		TransactionBody* TransactionBody::createGroupFriendsUpdate(bool colorFusion)
		{
			auto obj = new TransactionBody;
			auto groupFriendsUpdate = obj->mProtoTransactionBody->mutable_group_friends_update();
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
			auto registerAddress = obj->mProtoTransactionBody->mutable_register_address();
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
			auto creation = obj->mProtoTransactionBody->mutable_creation();
			creation->set_allocated_recipient(transferAmount.release());
			auto protoTargetDate = creation->mutable_target_date();
			DataTypeConverter::convertToProtoTimestampSeconds(targetDate.timestamp(), protoTargetDate);
			obj->initSpecificTransaction();
			return obj;
		}

		TransactionBody* TransactionBody::createTransactionTransfer(std::unique_ptr<proto::gradido::TransferAmount> transferAmount, const MemoryBin* recipientPubkey)
		{
			auto obj = new TransactionBody;
			auto transfer = obj->mProtoTransactionBody->mutable_transfer();
			transfer->set_allocated_sender(transferAmount.release());
			transfer->set_allocated_recipient(recipientPubkey->copyAsString().release());
			obj->initSpecificTransaction();
			return obj;
		}

		void TransactionBody::updateToOutbound(const std::string& otherGroup)
		{
			LOCK_RECURSIVE;
			mProtoTransactionBody->set_other_group(otherGroup);
			mProtoTransactionBody->set_type(proto::gradido::TransactionBody_CrossGroupType_OUTBOUND);
		}

		void TransactionBody::updateToInbound(const std::string& otherGroup)
		{
			LOCK_RECURSIVE;
			mProtoTransactionBody->set_other_group(otherGroup);
			mProtoTransactionBody->set_type(proto::gradido::TransactionBody_CrossGroupType_INBOUND);
		}

		proto::gradido::TransactionBody_CrossGroupType TransactionBody::getCrossGroupType() const
		{
			// cannot inline, because this doens't work in dll build
			return mProtoTransactionBody->type();
		}
		const std::string& TransactionBody::getVersionNumber() const
		{
			// cannot inline, because this doens't work in dll build
			return mProtoTransactionBody->version_number();
		}
		const std::string& TransactionBody::getOtherGroup() const
		{
			// cannot inline, because this doens't work in dll build
			return mProtoTransactionBody->other_group();
		}

		std::string TransactionBody::getMemo() const
		{
			LOCK_RECURSIVE;
			if (mProtoTransactionBody->IsInitialized()) {
				std::string result(mProtoTransactionBody->memo());

				return result;
			}
			return "<uninitalized>";
		}

		std::string TransactionBody::getGroupId(const IGradidoBlockchain* blockchain) const
		{
			std::string color = TransactionEntry::getCoinGroupId(this);
			if (!color.size()) {
				return blockchain->getGroupId();
			}
			return std::move(color);
		}

		void TransactionBody::setMemo(const std::string& memo)
		{
			LOCK_RECURSIVE;
			mProtoTransactionBody->set_memo(memo);
		}

		std::unique_ptr<std::string> TransactionBody::getBodyBytes() const
		{
			LOCK_RECURSIVE;
			assert(mProtoTransactionBody->IsInitialized());

			auto size = mProtoTransactionBody->ByteSizeLong();
			//auto bodyBytesSize = MemoryManager::getInstance()->getFreeMemory(mProtoCreation.ByteSizeLong());
			std::string* resultString(new std::string(size, 0));
			if (!mProtoTransactionBody->SerializeToString(resultString)) {
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
			case TRANSACTION_DEFERRED_TRANSFER: return "Deferred Transaction";
			}
			return "<unknown>";
		}

		const DeferredTransfer* TransactionBody::getDeferredTransfer() const
		{
			return dynamic_cast<DeferredTransfer*>(mTransactionSpecific);
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

			try {
				if ((level & TRANSACTION_VALIDATION_SINGLE) == TRANSACTION_VALIDATION_SINGLE) {
					if (getVersionNumber() != GRADIDO_PROTOCOL_VERSION) {
						throw TransactionValidationInvalidInputException("wrong version", "version_number", "uint64");
					}
					// memo is only mandatory for transfer and creation transactions
					if (isDeferredTransfer() || isTransfer() || isCreation()) {
						if (getMemo().size() < 5 || getMemo().size() > 350) {
							throw TransactionValidationInvalidInputException("not in expected range [5;350]", "memo", "string");
						}
					}
					auto otherGroup = getOtherGroup();
					if (otherGroup.size() && !TransactionBase::isValidGroupAlias(getOtherGroup())) {
						throw TransactionValidationInvalidInputException("invalid character, only ascii", "other_group", "string");
					}
					if (isDeferredTransfer()) {
						auto deferredTransfer = getDeferredTransfer();
						auto timeout = deferredTransfer->getTimeoutAsPocoTimestamp();
						if (getCreatedSeconds() >= timeout.epochTime()) {
							throw TransactionValidationInvalidInputException("already reached", "timeout", "Timestamp");
						}
					}
					// check target date for creation transactions
					if (isCreation()) {
						getCreationTransaction()->validateTargetDate(getCreatedSeconds());
					}
				}

				mTransactionSpecific->validate(level, blockchain, parentGradidoBlock);
			}
			catch (TransactionValidationException& ex) {
				ex.setTransactionBody(this);
				throw;
			}

			return true;

		}

		bool TransactionBody::isBelongToUs(const TransactionBody* pairingTransaction) const
		{
			if (getMemo() != pairingTransaction->getMemo()) {
				return false;
			}
			if (getCreatedSeconds() != pairingTransaction->getCreatedSeconds()) {
				return false;
			}
			if (getOtherGroup() == pairingTransaction->getOtherGroup()) {
				return false;
			}
			return getTransactionBase()->isBelongToUs(pairingTransaction->getTransactionBase());
		}

		std::string TransactionBody::toDebugString() const
		{
			std::string result;
			Poco::DateTime createdDate = Poco::Timestamp(getCreatedSeconds() * Poco::Timestamp::resolution());
			auto createdDateString = Poco::DateTimeFormatter::format(createdDate, Poco::DateTimeFormat::SORTABLE_FORMAT);
			result = "created: " + createdDateString + "\n";
			result += getTransactionBase()->toDebugString();
			return std::move(result);
		}

		void TransactionBody::initSpecificTransaction()
		{
			// check Type
			if (mProtoTransactionBody->has_creation()) {
				mTransactionType = TRANSACTION_CREATION;
				mTransactionSpecific = new model::gradido::TransactionCreation(mProtoTransactionBody->creation());
			}
			else if (mProtoTransactionBody->has_transfer()) {
				mTransactionType = TRANSACTION_TRANSFER;
				mTransactionSpecific = new model::gradido::TransactionTransfer(mProtoTransactionBody->transfer());
			}
			else if (mProtoTransactionBody->has_group_friends_update()) {
				mTransactionType = TRANSACTION_GROUP_FRIENDS_UPDATE;
				mTransactionSpecific = new model::gradido::GroupFriendsUpdate(mProtoTransactionBody->group_friends_update());
			}
			else if (mProtoTransactionBody->has_register_address()) {
				mTransactionType = TRANSACTION_REGISTER_ADDRESS;
				mTransactionSpecific = new model::gradido::RegisterAddress(mProtoTransactionBody->register_address());
			}
			else if (mProtoTransactionBody->has_deferred_transfer()) {
				mTransactionType = TRANSACTION_DEFERRED_TRANSFER;
				mTransactionSpecific = new model::gradido::DeferredTransfer(mProtoTransactionBody->deferred_transfer());
			}
			mTransactionSpecific->prepare();
		}



	}
}
