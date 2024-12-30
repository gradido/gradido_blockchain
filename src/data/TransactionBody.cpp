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
			else if (isRedeemDeferredTransfer()) return data::TransactionType::REDEEM_DEFERRED_TRANSFER;
			else if (isTimeoutDeferredTransfer()) return data::TransactionType::TIMEOUT_DEFERRED_TRANSFER;
			return data::TransactionType::NONE;
		}

		bool TransactionBody::isPairing(const TransactionBody& other) const
		{
			// memo, type and createdAt must be the same, otherGroup must be different
			if (mMemos != other.mMemos ||
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
			if (isDeferredTransfer() && other.isDeferredTransfer()) {
				return *mDeferredTransfer == *other.mDeferredTransfer;
			}
			if (isRedeemDeferredTransfer() && other.isRedeemDeferredTransfer()) {
				return *mRedeemDeferredTransfer == *other.mRedeemDeferredTransfer;
			}
			if (isTimeoutDeferredTransfer() && other.isTimeoutDeferredTransfer()) {
				return *mTimeoutDeferredTransfer == *other.mTimeoutDeferredTransfer;
			}
			return false;
		}

		bool TransactionBody::isInvolved(const memory::Block& publicKey) const
		{
			if (isCommunityRoot()) return mCommunityRoot->isInvolved(publicKey);
			if (isRegisterAddress()) return mRegisterAddress->isInvolved(publicKey);
			if (isTransfer()) return mTransfer->isInvolved(publicKey);
			if (isCreation()) return mCreation->isInvolved(publicKey);
			if (isDeferredTransfer()) return mDeferredTransfer->isInvolved(publicKey);
			if (isRedeemDeferredTransfer()) return mRedeemDeferredTransfer->isInvolved(publicKey);
			return false;
		}

		const TransferAmount& TransactionBody::getTransferAmount() const
		{
			if (isTransfer()) { return mTransfer->getSender(); }
			else if (isDeferredTransfer()) { return mDeferredTransfer->getTransfer().getSender(); }
			else if (isRedeemDeferredTransfer()) { return mRedeemDeferredTransfer->getTransfer().getSender(); }
			else if (isCreation()) { return mCreation->getRecipient(); }
			throw GradidoNodeInvalidDataException("cannnot provide transferAmount for this transaction type");
		}

		std::vector<memory::ConstBlockPtr> TransactionBody::getInvolvedAddresses() const
		{
			if (isCommunityFriendsUpdate()) return {};
			if (isCommunityRoot()) return mCommunityRoot->getInvolvedAddresses();
			if (isRegisterAddress()) return mRegisterAddress->getInvolvedAddresses();
			if (isTransfer()) return mTransfer->getInvolvedAddresses();
			if (isCreation()) return mCreation->getInvolvedAddresses();
			if (isDeferredTransfer()) return mDeferredTransfer->getInvolvedAddresses();
			if (isRedeemDeferredTransfer()) return mRedeemDeferredTransfer->getInvolvedAddresses();
			return {};
		}
	}
}