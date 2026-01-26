#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/data/TransactionBody.h"
#include "gradido_blockchain/data/TransactionType.h"
#include "gradido_blockchain/memory/Block.h"

#include <memory>
#include <vector>

using memory::Block, memory::ConstBlockPtr;
using std::shared_ptr, std::make_shared;
using std::vector;

namespace gradido {
	namespace data {

		TransactionType TransactionBody::getTransactionType() const
		{
			if (isTransfer()) return TransactionType::TRANSFER;
			else if (isCreation()) return TransactionType::CREATION;
			else if (isCommunityFriendsUpdate()) return TransactionType::COMMUNITY_FRIENDS_UPDATE;
			else if (isRegisterAddress()) return TransactionType::REGISTER_ADDRESS;
			else if (isDeferredTransfer()) return TransactionType::DEFERRED_TRANSFER;
			else if (isCommunityRoot()) return TransactionType::COMMUNITY_ROOT;
			else if (isRedeemDeferredTransfer()) return TransactionType::REDEEM_DEFERRED_TRANSFER;
			else if (isTimeoutDeferredTransfer()) return TransactionType::TIMEOUT_DEFERRED_TRANSFER;
			return TransactionType::NONE;
		}

		bool TransactionBody::isPairing(const TransactionBody& other) const
		{
			// memo, type and createdAt must be the same, otherGroup must be different
			if (mMemos != other.mMemos ||
				mCreatedAt != other.mCreatedAt ||
				mOtherCommunityIdIndex == other.mOtherCommunityIdIndex
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
				return mTransfer->isPairing(*other.mTransfer);
			}
			if (isDeferredTransfer() && other.isDeferredTransfer()) {
				return *mDeferredTransfer == *other.mDeferredTransfer;
			}
			if (isRedeemDeferredTransfer() && other.isRedeemDeferredTransfer()) {
				return mRedeemDeferredTransfer->isPairing(*other.mRedeemDeferredTransfer);
			}
			if (isTimeoutDeferredTransfer() && other.isTimeoutDeferredTransfer()) {
				return *mTimeoutDeferredTransfer == *other.mTimeoutDeferredTransfer;
			}
			return false;
		}

		bool TransactionBody::isInvolved(const Block& publicKey) const
		{
			if (isCommunityRoot()) {
				assert(publicKey.size() == 32);
				auto publicKeyIndex = g_appContext->getOrAddPublicKeyIndex(mCommunityIdIndex, { publicKey.data() });
				return mSpecific.communityRoot.isInvolved({mCommunityIdIndex,publicKeyIndex});
			}
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
		bool TransactionBody::hasTransferAmount() const
		{
			return isTransfer() || isDeferredTransfer() || isRedeemDeferredTransfer() || isCreation();
		}

		vector<ConstBlockPtr> TransactionBody::getInvolvedAddresses() const
		{
			if (isCommunityFriendsUpdate()) return {};
			if (isCommunityRoot()) {				
				return {
					make_shared<const Block>(g_appContext->getPublicKey(mSpecific.communityRoot.publicKeyIndex).value().data()),
					make_shared<const Block>(g_appContext->getPublicKey(mSpecific.communityRoot.gmwPublicKeyIndex).value().data()),
					make_shared<const Block>(g_appContext->getPublicKey(mSpecific.communityRoot.aufPublicKeyIndex).value().data())
				};
			}
			if (isRegisterAddress()) return mRegisterAddress->getInvolvedAddresses();
			if (isTransfer()) return mTransfer->getInvolvedAddresses();
			if (isCreation()) return mCreation->getInvolvedAddresses();
			if (isDeferredTransfer()) return mDeferredTransfer->getInvolvedAddresses();
			if (isRedeemDeferredTransfer()) return mRedeemDeferredTransfer->getInvolvedAddresses();
			return {};
		}
	}
}