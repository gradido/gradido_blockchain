#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/const.h"
#include "gradido_blockchain/data/adapter/MemoryBlock.h"
#include "gradido_blockchain/data/adapter/Types.h"
#include "gradido_blockchain/data/adapter/Timestamp.h"
#include "gradido_blockchain/data/adapter/TransactionBody.h"
#include "gradido_blockchain/data/TransactionBody.h"
#include "gradido_blockchain/lib/DictionaryExceptions.h"
#include "gradido_blockchain/memory/Block.h"

#include "gradido_protobuf_zig.h"
#include "magic_enum/magic_enum.hpp"

#include <memory>
#include <string>
#include <vector>

using namespace magic_enum;
using memory::Block, memory::ConstBlockPtr;
using std::shared_ptr, std::make_shared;
using std::to_string;
using std::vector;

namespace gradido {
	namespace data {
		using adapter::fromGrdw, adapter::toGrdw;

		ConstTransactionBodyPtr TransactionBody::fromGrdwTransactionBody(grdw_transaction_body* grdw_body, uint32_t communityIdIndex)
		{
			assert(grdw_body);
			if (strcmp(grdw_body->version_number, GRADIDO_TRANSACTION_BODY_VERSION_STRING)) {
				throw GradidoNodeInvalidDataException("invalid version of grdw_body");
			}
			auto result = make_shared<TransactionBody>();
			if (grdw_body->memos_count) {
				result->mMemos.reserve(grdw_body->memos_count);
				for (uint8_t i = 0; i < grdw_body->memos_count; i++) {
					auto& memo = grdw_body->memos[i];
					result->mMemos.emplace_back(fromGrdw(memo.type), Block(memo.memo_size, memo.memo));
				}				
			}
			result->mCreatedAt = fromGrdw(grdw_body->created_at);
			result->mType = fromGrdw(grdw_body->type);
			result->mTransactionType = fromGrdw(grdw_body->transaction_type);
			result->mCommunityIdIndex = communityIdIndex;
			if (grdw_body->other_group) {
				result->mOtherCommunityIdIndex = g_appContext->getOrAddCommunityIdIndex(grdw_body->other_group);
			}
			switch (result->mTransactionType) {
			case TransactionType::TRANSFER: 
				result->mSpecific = make_shared<GradidoTransfer>(
					fromGrdw(grdw_body->data.transfer->sender, communityIdIndex),
					fromGrdw(grdw_body->data.transfer->recipient)
				);
				break;
			case TransactionType::CREATION: 
				result->mSpecific = make_shared<GradidoCreation>(
					fromGrdw(grdw_body->data.creation->recipient, communityIdIndex),
					fromGrdw(grdw_body->data.creation->target_date)
				);
				break;
			case TransactionType::REGISTER_ADDRESS:
				result->mSpecific = make_shared<RegisterAddress>(
					fromGrdw(grdw_body->data.register_address->address_type),
					grdw_body->data.register_address->derivation_index,
					fromGrdw(grdw_body->data.register_address->user_pubkey),
					fromGrdw(grdw_body->data.register_address->name_hash),
					fromGrdw(grdw_body->data.register_address->account_pubkey)
				);
				break;
			case TransactionType::DEFERRED_TRANSFER: 
				result->mSpecific = make_shared<GradidoDeferredTransfer>(
					GradidoTransfer(
						fromGrdw(grdw_body->data.deferred_transfer->transfer.sender, communityIdIndex),
						fromGrdw(grdw_body->data.deferred_transfer->transfer.recipient)
					),
					grdw_body->data.deferred_transfer->timeout_duration
				);
				break;
			case TransactionType::REDEEM_DEFERRED_TRANSFER:
				result->mSpecific = make_shared<GradidoRedeemDeferredTransfer>(
					grdw_body->data.redeem_deferred_transfer->deferred_transfer_transaction_nr,
					GradidoTransfer(
						fromGrdw(grdw_body->data.redeem_deferred_transfer->transfer.sender, communityIdIndex),
						fromGrdw(grdw_body->data.redeem_deferred_transfer->transfer.recipient)
					)
				);
				break;
			case TransactionType::TIMEOUT_DEFERRED_TRANSFER: 
				result->mSpecific = make_shared<GradidoTimeoutDeferredTransfer>(
					grdw_body->data.timeout_deferred_transfer->deferred_transfer_transaction_nr
				);
				break;
			case TransactionType::COMMUNITY_ROOT:
				result->mSpecific = make_shared<CommunityRoot>(
					fromGrdw(grdw_body->data.community_root->pubkey),
					fromGrdw(grdw_body->data.community_root->gmw_pubkey),
					fromGrdw(grdw_body->data.community_root->auf_pubkey)
				);
				break;
			case TransactionType::COMMUNITY_FRIENDS_UPDATE:
				result->mSpecific = make_shared<CommunityFriendsUpdate>(grdw_body->data.community_friends_update->color_fusion);
				break;
			default: throw GradidoUnhandledEnum("missing implementation for TransactionBody::fromGrdw", "TransactionType", to_string(grdw_body->transaction_type).c_str());
			}
			return result;
		}

		void TransactionBody::toGrdw(grdw_transaction_body* grdw_body) const
		{
			grdw_body->version_number = grdu_reserve_copy_string(GRADIDO_TRANSACTION_BODY_VERSION_STRING, grdu_strlen(GRADIDO_TRANSACTION_BODY_VERSION_STRING));
			if (mMemos.size()) {
				grdw_transaction_body_reserve_memos(grdw_body, mMemos.size());
				for (int i = 0; i < mMemos.size(); ++i) {
					const auto& memo = mMemos[i];
					auto grdw_memo = &grdw_body->memos[i];
					grdw_memo->type = adapter::toGrdw(memo.getKeyType());
					grdw_memo->memo_size = memo.getMemo().size();
					grdw_memo->memo = grdu_reserve_copy(memo.getMemo().data(), memo.getMemo().size());
				}
			}
			else {
				grdw_body->memos_count = 0;
				grdw_body->memos = nullptr;
			}
			if (mOtherCommunityIdIndex) {
				auto otherCommunityId = g_appContext->getCommunityIds().getDataForIndex(mOtherCommunityIdIndex.value());
				if (!otherCommunityId) {
					throw DictionaryMissingEntryException("missing other community id", to_string(mOtherCommunityIdIndex.value()));
				}
				grdw_body->other_group = grdu_reserve_copy_string(otherCommunityId->data(), otherCommunityId->size());
			}
			else {
				grdw_body->other_group = nullptr;
			}
			grdw_body->created_at = adapter::toGrdw(mCreatedAt);
			grdw_body->type = adapter::toGrdw(mType);
			grdw_body->transaction_type = adapter::toGrdw(mTransactionType);
			grdw_gradido_transfer grdwTransfer;
			
			if (TransactionType::REGISTER_ADDRESS == mTransactionType) {
				auto registerAddress = getRegisterAddress();
				auto accountPubkey = registerAddress->getAccountPublicKey();
				auto userPubkey = registerAddress->getUserPublicKey();
				auto nameHash = registerAddress->getNameHash();
				if (!accountPubkey || accountPubkey->size() != 32 || !nameHash || nameHash->size() != 32 || !userPubkey || userPubkey->size() != 32) {
					throw GradidoNodeInvalidDataException("at least one of account public key, name hash, user public key isn't 32 Bytes");
				}
				grdw_body->data.register_address = grdw_register_address_new(
					userPubkey->data(),
					adapter::toGrdw(getRegisterAddress()->getAddressType()),
					nameHash->data(),
					accountPubkey->data(),
					getRegisterAddress()->getDerivationIndex()
				);
			}

			switch (mTransactionType) {
			case TransactionType::TRANSFER: 
				grdw_body->data.transfer = grdw_gradido_transfer_new(
					adapter::toGrdw(getTransferAmount(), mCommunityIdIndex),
					getTransfer()->getRecipient()->data()
				);
				break;
			case TransactionType::CREATION: 
				grdw_body->data.creation = grdw_gradido_creation_new(
					adapter::toGrdw(getTransferAmount(), mCommunityIdIndex),
					adapter::toGrdw(getCreation()->getTargetDate())
				);
				break;
			case TransactionType::REGISTER_ADDRESS: break;
			case TransactionType::DEFERRED_TRANSFER:
				assert(getDeferredTransfer()->getRecipientPublicKey()->size() == 32);
				grdwTransfer.sender = adapter::toGrdw(getTransferAmount(), mCommunityIdIndex);
				memcpy(grdwTransfer.recipient, getDeferredTransfer()->getRecipientPublicKey()->data(), 32);
				grdw_body->data.deferred_transfer = grdw_gradido_deferred_transfer_new(
					grdwTransfer,
					getDeferredTransfer()->getTimeoutDuration().getSeconds()
				);
				break;
			case TransactionType::REDEEM_DEFERRED_TRANSFER:
				assert(getRedeemDeferredTransfer()->getRecipientPublicKey()->size() == 32);
				grdwTransfer.sender = adapter::toGrdw(getTransferAmount(), mCommunityIdIndex);
				memcpy(grdwTransfer.recipient, getRedeemDeferredTransfer()->getRecipientPublicKey()->data(), 32);
				grdw_body->data.redeem_deferred_transfer = grdw_gradido_redeem_deferred_transfer_new(
					getRedeemDeferredTransfer()->getDeferredTransferTransactionNr(),
					grdwTransfer
				);
				break;
			case TransactionType::TIMEOUT_DEFERRED_TRANSFER:
				grdw_body->data.timeout_deferred_transfer = grdw_gradido_timeout_deferred_transfer_new(
					getTimeoutDeferredTransfer()->getDeferredTransferTransactionNr()
				);
				break;
			case TransactionType::COMMUNITY_ROOT:
				grdw_body->data.community_root = grdw_community_root_new(
					getCommunityRoot()->getPublicKey()->data(),
					getCommunityRoot()->getGmwPubkey()->data(),
					getCommunityRoot()->getAufPubkey()->data()
				);
				break;
			case TransactionType::COMMUNITY_FRIENDS_UPDATE:
				grdw_body->data.community_friends_update = grdw_community_friends_update_new(getCommunityFriendsUpdate()->getColorFusion());
				break;
			default: throw GradidoUnhandledEnum("missing implementation for TransactionBody::toGrdw", "TransactionType", to_string(static_cast<int>(mTransactionType)).c_str());
			}
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
				return *getCommunityFriendsUpdate() == *other.getCommunityFriendsUpdate();
			}
			if (isRegisterAddress() && other.isRegisterAddress()) {
				return *getRegisterAddress() == *other.getRegisterAddress();
			}
			if (isTransfer() && other.isTransfer()) {
				return getTransfer()->isPairing(*other.getTransfer());
			}
			if (isDeferredTransfer() && other.isDeferredTransfer()) {
				return *getDeferredTransfer() == *other.getDeferredTransfer();
			}
			if (isRedeemDeferredTransfer() && other.isRedeemDeferredTransfer()) {
				return getRedeemDeferredTransfer()->isPairing(*other.getRedeemDeferredTransfer());
			}
			if (isTimeoutDeferredTransfer() && other.isTimeoutDeferredTransfer()) {
				return *getTimeoutDeferredTransfer() == *other.getTimeoutDeferredTransfer();
			}
			return false;
		}

		bool TransactionBody::isInvolved(const Block& publicKey) const
		{
			if (isCommunityRoot()) return getCommunityRoot()->isInvolved(publicKey);
			if (isRegisterAddress()) return getRegisterAddress()->isInvolved(publicKey);
			if (isTransfer()) return getTransfer()->isInvolved(publicKey);
			if (isCreation()) return getCreation()->isInvolved(publicKey);
			if (isDeferredTransfer()) return getDeferredTransfer()->isInvolved(publicKey);
			if (isRedeemDeferredTransfer()) return getRedeemDeferredTransfer()->isInvolved(publicKey);
			return false;
		}

		const TransferAmount& TransactionBody::getTransferAmount() const
		{
			if (isTransfer()) { return getTransfer()->getSender(); }
			else if (isDeferredTransfer()) { return getDeferredTransfer()->getTransfer().getSender(); }
			else if (isRedeemDeferredTransfer()) { return getRedeemDeferredTransfer()->getTransfer().getSender(); }
			else if (isCreation()) { return getCreation()->getRecipient(); }
			throw GradidoNodeInvalidDataException("cannnot provide transferAmount for this transaction type");
		}
		bool TransactionBody::hasTransferAmount() const
		{
			return isTransfer() || isDeferredTransfer() || isRedeemDeferredTransfer() || isCreation();
		}

		vector<ConstBlockPtr> TransactionBody::getInvolvedAddresses() const
		{
			if (isCommunityFriendsUpdate()) return {};
			if (isCommunityRoot()) return getCommunityRoot()->getInvolvedAddresses();
			if (isRegisterAddress()) return getRegisterAddress()->getInvolvedAddresses();
			if (isTransfer()) return getTransfer()->getInvolvedAddresses();
			if (isCreation()) return getCreation()->getInvolvedAddresses();
			if (isDeferredTransfer()) return getDeferredTransfer()->getInvolvedAddresses();
			if (isRedeemDeferredTransfer()) return getRedeemDeferredTransfer()->getInvolvedAddresses();
			return {};
		}
	}
}