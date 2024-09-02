#include "gradido_blockchain/data/TransactionBody.h"

namespace gradido {
	namespace data {

		TransactionType TransactionBody::getTransactionType() const
		{
			if (isTransfer()) return data::TransactionType::TRANSFER;
			else if (isCreation()) return data::TransactionType::CREATION;
			else if (isCommunityFriendsUpdate()) return data::TransactionType::COMMUNITY_FRIENDS_UPDATE;
			else if (isRegisterAddress()) return data::TransactionType::REGISTER_ADDRESS;
			else if (isDeferredTransfer()) return data::TransactionType::DEFERRED_TRANSFER;
			else if (isCommunityRoot()) return data::TransactionType::COMMUNITY_ROOT;
			return data::TransactionType::NONE;
		}

		bool TransactionBody::isPairing(const TransactionBody& other) const
		{
			// memo, type and createdAt must be the same, otherGroup must be different
			if (mMemo != other.mMemo ||
				mCreatedAt != other.mCreatedAt ||
				mOtherGroup == other.mOtherGroup
				) {
				return false;
			}
			if (isCommunityFriendsUpdate() && other.isCommunityFriendsUpdate()) {
				return *mCommunityFriendsUpdate == *other.mCommunityFriendsUpdate;
			}
			if (isRegisterAddress() && other.isRegisterAddress()) {
				return *mRegisterAddress == *other.mRegisterAddress;
			}
			if (isTransfer() && other.isTransfer()) {
				return *mTransfer == *other.mTransfer;
			}
			return false;
		}

		bool TransactionBody::isInvolved(memory::ConstBlockPtr publicKey) const
		{
			if (isCommunityRoot()) return mCommunityRoot->isInvolved(publicKey);
			if (isRegisterAddress()) return mRegisterAddress->isInvolved(publicKey);
			if (isTransfer()) return mTransfer->isInvolved(publicKey);
			if (isCreation()) return mCreation->isInvolved(publicKey);
			if (isDeferredTransfer()) return mDeferredTransfer->isInvolved(publicKey);
			return false;
		}

		const TransferAmount* TransactionBody::getTransferAmount() const
		{
			if (isTransfer()) { return &mTransfer->getSender(); }
			else if (isDeferredTransfer()) { return &mDeferredTransfer->getTransfer().getSender(); }
			else if (isCreation()) { return &mCreation->getRecipient(); }
			return nullptr;
		}

		std::vector<memory::ConstBlockPtr> TransactionBody::getInvolvedAddresses() const
		{
			if (isCommunityFriendsUpdate()) return {};
			if (isCommunityRoot()) return mCommunityRoot->getInvolvedAddresses();
			if (isRegisterAddress()) return mRegisterAddress->getInvolvedAddresses();
			if (isTransfer()) return mTransfer->getInvolvedAddresses();
			if (isCreation()) return mCreation->getInvolvedAddresses();
			if (isDeferredTransfer()) return mDeferredTransfer->getInvolvedAddresses();
			return {};
		}
	}
}