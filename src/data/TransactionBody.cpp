#include "gradido_blockchain_core/data/wire/specific_transactions.h"
#include "gradido_blockchain_core/data/wire/transaction_body.h"
#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/const.h"
#include "gradido_blockchain/data/adapter/memoryBlock.h"
#include "gradido_blockchain/data/adapter/publicKey.h"
#include "gradido_blockchain/data/adapter/types.h"
#include "gradido_blockchain/data/adapter/timestamp.h"
#include "gradido_blockchain/data/adapter/transactionBody.h"
#include "gradido_blockchain/data/compact/CommunityRootTx.h"
#include "gradido_blockchain/data/compact/PublicKeyIndex.h"
#include "gradido_blockchain/data/compact/RegisterAddressTx.h"
#include "gradido_blockchain/data/CrossGroupType.h"
#include "gradido_blockchain/data/TransactionBody.h"
#include "gradido_blockchain/data/TransactionType.h"
#include "gradido_blockchain/lib/DictionaryExceptions.h"
#include "gradido_blockchain/memory/Block.h"

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
		using adapter::toPublicKeyIndex, adapter::toConstBlockPtr;
		using compact::PublicKeyIndex;

		ConstTransactionBodyPtr TransactionBody::fromGrdw(grdw_transaction_body* grdw_body, uint32_t communityIdIndex)
		{
			assert(grdw_body);
			auto result = make_shared<TransactionBody>();
			if (grdw_body->memos_count) {
				result->mMemos.reserve(grdw_body->memos_count);
				for (uint8_t i = 0; i < grdw_body->memos_count; i++) {
					auto& memo = grdw_body->memos[i];
					result->mMemos.emplace_back(adapter::fromGrdw(memo.type), Block(memo.memo.size, memo.memo.data));
				}				
			}
			result->mCreatedAt = adapter::fromGrdw(grdw_body->created_at);
			result->mType = adapter::fromGrdw(grdw_body->type);
			result->mTransactionType = adapter::fromGrdw(grdw_body->transaction_type);
			result->mCommunityIdIndex = communityIdIndex;
			if (grdw_body->other_community_uuid) {
				result->mOtherCommunityIdIndex = g_appContext->getOrAddCommunityIdIndex(Uuid(grdw_body->other_community_uuid));
			}
			switch (result->mTransactionType) {
			case TransactionType::TRANSFER: 
				result->mSpecific = make_shared<GradidoTransfer>(
					adapter::fromGrdw(grdw_body->transfer.sender, communityIdIndex),
					adapter::fromGrdw(grdw_body->transfer.recipient)
				);
				break;
			case TransactionType::CREATION: 
				result->mSpecific = make_shared<GradidoCreation>(
					adapter::fromGrdw(grdw_body->creation.recipient, communityIdIndex),
					adapter::fromGrdw(grdw_body->creation.target_date)
				);
				break;
			case TransactionType::REGISTER_ADDRESS:
				result->mSpecific = compact::RegisterAddressTx::fromGrdw(&grdw_body->register_address, communityIdIndex);
				break;
			case TransactionType::DEFERRED_TRANSFER: 
				result->mSpecific = make_shared<GradidoDeferredTransfer>(
					GradidoTransfer(
						adapter::fromGrdw(grdw_body->deferred_transfer.transfer.sender, communityIdIndex),
						adapter::fromGrdw(grdw_body->deferred_transfer.transfer.recipient)
					),
					grdw_body->deferred_transfer.timeout_duration
				);
				break;
			case TransactionType::REDEEM_DEFERRED_TRANSFER:
				result->mSpecific = make_shared<GradidoRedeemDeferredTransfer>(
					grdw_body->redeem_deferred_transfer.deferred_transfer_transaction_nr,
					GradidoTransfer(
						adapter::fromGrdw(grdw_body->redeem_deferred_transfer.transfer.sender, communityIdIndex),
						adapter::fromGrdw(grdw_body->redeem_deferred_transfer.transfer.recipient)
					)
				);
				break;
			case TransactionType::TIMEOUT_DEFERRED_TRANSFER: 
				result->mSpecific = make_shared<GradidoTimeoutDeferredTransfer>(
					grdw_body->timeout_deferred_transfer.deferred_transfer_transaction_nr
				);
				break;
			case TransactionType::COMMUNITY_ROOT:
				result->mSpecific = compact::CommunityRootTx::fromGrdw(&grdw_body->community_root, communityIdIndex);
				break;
			case TransactionType::COMMUNITY_FRIENDS_UPDATE:
				result->mSpecific = make_shared<CommunityFriendsUpdate>(grdw_body->community_friends_update.color_fusion);
				break;
			default: throw GradidoUnhandledEnum("missing implementation for TransactionBody::fromGrdw", "TransactionType", to_string(grdw_body->transaction_type).c_str());
			}
			return result;
		}

		void TransactionBody::toGrdw(grd_memory* alloc, grdw_transaction_body* grdw_body) const
		{
			if (mMemos.size()) {
				grdw_transaction_body_reserve_memos(grdw_body, mMemos.size(), alloc);
				if (grdw_body->memos) {
					for (int i = 0; i < mMemos.size(); ++i) {
						const auto& memo = mMemos[i];
						auto grdw_memo = &grdw_body->memos[i];
						grdw_memo->type = adapter::toGrdw(memo.getKeyType());
						// maybe use reference instead of copy, but then it is important to set ptr to zero before calling free on grdw body
						grd_memory_block_alloc(&grdw_memo->memo, alloc, memo.getMemo().size());
						memcpy(grdw_memo->memo.data, memo.getMemo().data(), memo.getMemo().size());
					}
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
				Uuid otherCommunityUuid(otherCommunityId.value());
				grd_memory_buffer_alloc((uint8_t**)&grdw_body->other_community_uuid, alloc, 16);
				memcpy(grdw_body->other_community_uuid, otherCommunityUuid.data(), 16);
			}
			else {
				grdw_body->other_community_uuid = nullptr;
			}
			grdw_body->created_at = adapter::toGrdw(mCreatedAt);
			grdw_body->type = adapter::toGrdw(mType);
			grdw_body->transaction_type = adapter::toGrdw(mTransactionType);
		
			if (TransactionType::REGISTER_ADDRESS == mTransactionType) {
				auto registerAddress = getRegisterAddress();
				auto accountPubkey = PublicKeyIndex{ .communityIdIndex = mCommunityIdIndex, .publicKeyIndex = registerAddress->accountPublicKeyIndex }.getRawKey();
				auto userPubkey = PublicKeyIndex{ .communityIdIndex = mCommunityIdIndex, .publicKeyIndex = registerAddress->userPublicKeyIndex }.getRawKey();
				auto nameHash = g_appContext->getUserNameHashs().getDataForIndexOrThrow(registerAddress->nameHashIndex);
				if (accountPubkey.isEmpty() || accountPubkey.size() != 32 || nameHash.isEmpty() || nameHash.size() != 32 || userPubkey.isEmpty() || userPubkey.size() != 32) {
					throw GradidoNodeInvalidDataException("at least one of account public key, name hash, user public key isn't 32 Bytes");
				}
				grdw_register_address_assemble(
					&grdw_body->register_address,
					userPubkey.data(),
					adapter::toGrdw(registerAddress->addressType),
					registerAddress->derivationIndex,
					nameHash.data(),
					accountPubkey.data()
				);
			}
			else if (TransactionType::TRANSFER == mTransactionType) {
				auto transferAmount = adapter::toGrdw(alloc, getTransferAmount(), mCommunityIdIndex);
				grdw_gradido_transfer_assemble(
					&grdw_body->transfer,
					transferAmount.pubkey,
					transferAmount.amount,
					transferAmount.community_uuid,
					getTransfer()->getRecipient()->data()
				);
			}
			else if (TransactionType::CREATION == mTransactionType) {
				auto transferAmount = adapter::toGrdw(alloc, getTransferAmount(), mCommunityIdIndex);
				grdw_gradido_creation_assemble(
					&grdw_body->creation,
					transferAmount.pubkey,
					transferAmount.amount,
					transferAmount.community_uuid,
					getCreation()->getTargetDate().getSeconds()
				);
			}
			else if (TransactionType::DEFERRED_TRANSFER == mTransactionType) {
				auto transferAmount = adapter::toGrdw(alloc, getTransferAmount(), mCommunityIdIndex);
				grdw_gradido_deferred_transfer_assemble(
					&grdw_body->deferred_transfer,
					transferAmount.pubkey,
					transferAmount.amount,
					transferAmount.community_uuid,
					getDeferredTransfer()->getRecipientPublicKey()->data(),
					getDeferredTransfer()->getTimeoutDuration().getSeconds()
				);
			}
			else if (TransactionType::REDEEM_DEFERRED_TRANSFER == mTransactionType) {
				auto transferAmount = adapter::toGrdw(alloc, getTransferAmount(), mCommunityIdIndex);
				grdw_gradido_redeem_deferred_transfer_assemble(
					&grdw_body->redeem_deferred_transfer,
					getRedeemDeferredTransfer()->getDeferredTransferTransactionNr(),
					transferAmount.pubkey,
					transferAmount.amount,
					transferAmount.community_uuid,
					getRedeemDeferredTransfer()->getRecipientPublicKey()->data()
				);
			}
			
			switch (mTransactionType) {
			case TransactionType::TRANSFER: break;
			case TransactionType::CREATION: break;
			case TransactionType::REGISTER_ADDRESS: break;
			case TransactionType::DEFERRED_TRANSFER: break;
			case TransactionType::REDEEM_DEFERRED_TRANSFER: break;
			case TransactionType::TIMEOUT_DEFERRED_TRANSFER:
				grdw_gradido_timeout_deferred_transfer_assemble(
					&grdw_body->timeout_deferred_transfer,
					getTimeoutDeferredTransfer()->getDeferredTransferTransactionNr()
				);
				break;
			case TransactionType::COMMUNITY_ROOT:
				grdw_community_root_assemble(
					&grdw_body->community_root,
					PublicKeyIndex{ .communityIdIndex = mCommunityIdIndex, .publicKeyIndex = getCommunityRoot()->publicKeyIndex }.getRawKey().data(),
					PublicKeyIndex{ .communityIdIndex = mCommunityIdIndex, .publicKeyIndex = getCommunityRoot()->gmwPublicKeyIndex }.getRawKey().data(),
					PublicKeyIndex{ .communityIdIndex = mCommunityIdIndex, .publicKeyIndex = getCommunityRoot()->aufPublicKeyIndex }.getRawKey().data()
				);
				break;
			case TransactionType::COMMUNITY_FRIENDS_UPDATE:
				grdw_community_friends_update_assemble(
					&grdw_body->community_friends_update,
					getCommunityFriendsUpdate()->getColorFusion()
				);
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
			auto publicKeyIndex = toPublicKeyIndex(publicKey, mCommunityIdIndex).publicKeyIndex;
			if (isCommunityRoot()) {
				return
					getCommunityRoot()->publicKeyIndex == publicKeyIndex ||
					getCommunityRoot()->gmwPublicKeyIndex == publicKeyIndex ||
					getCommunityRoot()->aufPublicKeyIndex == publicKeyIndex;
			}
			if (isRegisterAddress()) {
				return
					getRegisterAddress()->accountPublicKeyIndex == publicKeyIndex ||
					getRegisterAddress()->userPublicKeyIndex == publicKeyIndex;
			}
			if (isTransfer()) return getTransfer()->isInvolved(publicKey);
			if (isCreation()) return getCreation()->isInvolved(publicKey);
			if (isDeferredTransfer()) return getDeferredTransfer()->isInvolved(publicKey);
			if (isRedeemDeferredTransfer()) return getRedeemDeferredTransfer()->isInvolved(publicKey);
			return false;
		}

		bool TransactionBody::isInvolved(compact::PublicKeyIndex publicKeyIndex) const
		{
			if (isCommunityRoot()) {
				return 
					getCommunityRoot()->publicKeyIndex == publicKeyIndex.publicKeyIndex ||
					getCommunityRoot()->gmwPublicKeyIndex == publicKeyIndex.publicKeyIndex ||
					getCommunityRoot()->aufPublicKeyIndex == publicKeyIndex.publicKeyIndex;
			}
			if (isRegisterAddress()) {
				return
					getRegisterAddress()->accountPublicKeyIndex == publicKeyIndex.publicKeyIndex ||
					getRegisterAddress()->userPublicKeyIndex == publicKeyIndex.publicKeyIndex;
			}
			if (isTransfer()) return getTransfer()->isInvolved(*toConstBlockPtr(publicKeyIndex));
			if (isCreation()) return getCreation()->isInvolved(*toConstBlockPtr(publicKeyIndex));
			if (isDeferredTransfer()) return getDeferredTransfer()->isInvolved(*toConstBlockPtr(publicKeyIndex));
			if (isRedeemDeferredTransfer()) return getRedeemDeferredTransfer()->isInvolved(*toConstBlockPtr(publicKeyIndex));
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
			if (isCommunityRoot()) {
				auto communityRoot = getCommunityRoot();
				return {
					toConstBlockPtr({.communityIdIndex = mCommunityIdIndex, .publicKeyIndex = communityRoot->publicKeyIndex}),
					toConstBlockPtr({.communityIdIndex = mCommunityIdIndex, .publicKeyIndex = communityRoot->gmwPublicKeyIndex}),
					toConstBlockPtr({.communityIdIndex = mCommunityIdIndex, .publicKeyIndex = communityRoot->aufPublicKeyIndex})
				};
			}
			if (isRegisterAddress()) {
				auto registerAddress = getRegisterAddress();
				return {
					toConstBlockPtr({.communityIdIndex = mCommunityIdIndex, .publicKeyIndex = registerAddress->accountPublicKeyIndex}),
					toConstBlockPtr({.communityIdIndex = mCommunityIdIndex, .publicKeyIndex = registerAddress->userPublicKeyIndex})
				};
			}
			if (isTransfer()) return getTransfer()->getInvolvedAddresses();
			if (isCreation()) return getCreation()->getInvolvedAddresses();
			if (isDeferredTransfer()) return getDeferredTransfer()->getInvolvedAddresses();
			if (isRedeemDeferredTransfer()) return getRedeemDeferredTransfer()->getInvolvedAddresses();
			return {};
		}

		
		vector<PublicKeyIndex> TransactionBody::getInvolvedAddressIndices() const
		{
			if (isCommunityFriendsUpdate()) return {};
			if (isCommunityRoot()) {
				auto communityRoot = getCommunityRoot();
				return {
					{.communityIdIndex = mCommunityIdIndex, .publicKeyIndex = communityRoot->publicKeyIndex},
					{.communityIdIndex = mCommunityIdIndex, .publicKeyIndex = communityRoot->gmwPublicKeyIndex},
					{.communityIdIndex = mCommunityIdIndex, .publicKeyIndex = communityRoot->aufPublicKeyIndex}
				};
			}
			if (isRegisterAddress()) {
				auto registerAddress = getRegisterAddress();
				return {
					{.communityIdIndex = mCommunityIdIndex, .publicKeyIndex = registerAddress->accountPublicKeyIndex},
					{.communityIdIndex = mCommunityIdIndex, .publicKeyIndex = registerAddress->userPublicKeyIndex}
				};
			}
			vector<PublicKeyIndex> result;
			if (isTransfer()) {
				result.reserve(2);
				fillFromGradidoTransfer(result, *getTransfer());
			}
			if (isCreation()) {
				// additional space for gmw, auf and moderator signature public key
				result.reserve(4);
				result.emplace_back(toPublicKeyIndex(getCreation()->getRecipient().getPublicKey(), mCommunityIdIndex));
			}
			if (isDeferredTransfer()) {
				result.reserve(2);
				fillFromGradidoTransfer(result, getDeferredTransfer()->getTransfer());
			}
			if (isRedeemDeferredTransfer()) {
				result.reserve(3);
				fillFromGradidoTransfer(result, getRedeemDeferredTransfer()->getTransfer());
			}
			if (isTimeoutDeferredTransfer()) {
				// contained in account balances
				result.reserve(2);
			}
			return result;
		}

		void TransactionBody::fillFromGradidoTransfer(std::vector<compact::PublicKeyIndex>& publicKeys, const GradidoTransfer& transfer) const
		{
			if (CrossGroupType::LOCAL != mType && !mOtherCommunityIdIndex) {
				throw GradidoNodeInvalidDataException("empty mOtherCommunityIdIndex in TransactionBody in CrossCommunityTransaction");
			}
			switch (mType) {
			case CrossGroupType::LOCAL: 
				publicKeys.emplace_back(toPublicKeyIndex(transfer.getSender().getPublicKey(), mCommunityIdIndex));
				publicKeys.emplace_back(toPublicKeyIndex(transfer.getRecipient(), mCommunityIdIndex));
				break;
			case CrossGroupType::OUTBOUND:
				publicKeys.emplace_back(toPublicKeyIndex(transfer.getSender().getPublicKey(), mCommunityIdIndex));
				publicKeys.emplace_back(toPublicKeyIndex(transfer.getRecipient(), *mOtherCommunityIdIndex));
				break;
			case CrossGroupType::INBOUND:
				publicKeys.emplace_back(toPublicKeyIndex(transfer.getSender().getPublicKey(), *mOtherCommunityIdIndex));
				publicKeys.emplace_back(toPublicKeyIndex(transfer.getRecipient(), mCommunityIdIndex));
				break;
			default: throw GradidoUnhandledEnum("TransactionBody fillFromGradidoTransfer", "CrossGroupType", enum_name(mType).data());
			}
		}
	}
}