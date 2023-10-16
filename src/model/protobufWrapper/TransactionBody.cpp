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
			auto createdAt = mProtoTransactionBody->mutable_created_at();
			DataTypeConverter::convertToProtoTimestamp(Poco::Timestamp(), createdAt);
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
		}

		void TransactionBody::setCreatedAt(Poco::DateTime createdAt)
		{
			LOCK_RECURSIVE;
			auto protoCreated = mProtoTransactionBody->mutable_created_at();
			DataTypeConverter::convertToProtoTimestamp(createdAt.timestamp(), protoCreated);
		}

		uint32_t TransactionBody::getCreatedAtSeconds() const
		{
			return mProtoTransactionBody->created_at().seconds();
		}

		Poco::DateTime TransactionBody::getCreatedAt() const
		{
			const auto& createdAt = mProtoTransactionBody->created_at();
			return Poco::Timestamp(
				createdAt.seconds() * Poco::Timestamp::resolution() + createdAt.nanos() / 1000
			);
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
			DataTypeConverter::convertToProtoTimestampSeconds(timeout, deferredTransfer->mutable_timeout());
			initSpecificTransaction();
		}


		TransactionBody* TransactionBody::createGroupFriendsUpdate(bool colorFusion)
		{
			auto obj = new TransactionBody;
			auto groupFriendsUpdate = obj->mProtoTransactionBody->mutable_community_friends_update();
			groupFriendsUpdate->set_color_fusion(colorFusion);
			obj->initSpecificTransaction();
			return obj;
		}

		TransactionBody* TransactionBody::createRegisterAddress(
			const MemoryBin* userPubkey,
			proto::gradido::RegisterAddress_AddressType type,
			const MemoryBin* nameHash,
			const MemoryBin* accountPubkey
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
			if (accountPubkey) {
				registerAddress->set_allocated_account_pubkey(accountPubkey->copyAsString().release());
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

		TransactionBody* TransactionBody::createCommunityRoot(const MemoryBin* pubkey, const MemoryBin* gmwPubkey, const MemoryBin* aufPubkey)
		{
			auto obj = new TransactionBody;
			auto communityRoot = obj->mProtoTransactionBody->mutable_community_root();
			communityRoot->set_allocated_pubkey(pubkey->copyAsString().release());
			communityRoot->set_allocated_gmw_pubkey(gmwPubkey->copyAsString().release());
			communityRoot->set_allocated_auf_pubkey(aufPubkey->copyAsString().release());
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

		const CommunityFriendsUpdate* TransactionBody::getGroupFriendsUpdate() const
		{
			return dynamic_cast<CommunityFriendsUpdate*>(mTransactionSpecific);
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

		const CommunityRoot* TransactionBody::getCommunityRoot() const
		{
			return dynamic_cast<CommunityRoot*>(mTransactionSpecific);
		}

		const TransactionBase* TransactionBody::getTransactionBase() const
		{
			return mTransactionSpecific;
		}

		bool TransactionBody::validate(
			TransactionValidationLevel level/* = TRANSACTION_VALIDATION_SINGLE*/,
			IGradidoBlockchain* blockchain/* = nullptr*/,
			const ConfirmedTransaction* parentConfirmedTransaction/* = nullptr*/) const
		{
			LOCK_RECURSIVE;

			try {
				if ((level & TRANSACTION_VALIDATION_SINGLE) == TRANSACTION_VALIDATION_SINGLE) {
					if (getVersionNumber() != GRADIDO_PROTOCOL_VERSION) {
						throw TransactionValidationInvalidInputException("wrong version", "version_number", "string");
					}
					// memo is only mandatory for transfer and creation transactions
					if (isDeferredTransfer() || isTransfer() || isCreation()) {
						if (getMemo().size() < 5 || getMemo().size() > 450) {
							throw TransactionValidationInvalidInputException("not in expected range [5;450]", "memo", "string");
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

				mTransactionSpecific->validate(level, blockchain, parentConfirmedTransaction);
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
			else if (mProtoTransactionBody->has_community_friends_update()) {
				mTransactionType = TRANSACTION_GROUP_FRIENDS_UPDATE;
				mTransactionSpecific = new model::gradido::CommunityFriendsUpdate(mProtoTransactionBody->community_friends_update());
			}
			else if (mProtoTransactionBody->has_register_address()) {
				mTransactionType = TRANSACTION_REGISTER_ADDRESS;
				mTransactionSpecific = new model::gradido::RegisterAddress(mProtoTransactionBody->register_address());
			}
			else if (mProtoTransactionBody->has_deferred_transfer()) {
				mTransactionType = TRANSACTION_DEFERRED_TRANSFER;
				mTransactionSpecific = new model::gradido::DeferredTransfer(mProtoTransactionBody->deferred_transfer());
			}
			else if (mProtoTransactionBody->has_community_root()) {
				mTransactionType = TRANSACTION_COMMUNITY_ROOT;
				mTransactionSpecific = new model::gradido::CommunityRoot(mProtoTransactionBody->community_root());
			} else {
				throw TransactionValidationInvalidInputException("transaction type unknown", "data", "oneof");
			}
			mTransactionSpecific->prepare();
		}



	}
}
