#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/data/TransactionBody.h"
#include "gradido_blockchain/interaction/serialize/TransactionBodyRole.h"
#include "gradido_blockchain/interaction/serialize/Exceptions.h"
#include "gradido_blockchain/lib/DictionaryExceptions.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"

#include "loguru/loguru.hpp"

#include <span>
#include <cstddef>

using std::byte;
using std::span;

namespace gradido {
	using namespace data;
	using namespace std;
	namespace interaction {
		namespace serialize {

			TransactionBodyMessage TransactionBodyRole::getMessage() const
			{
				auto createdAt = mBody.getCreatedAt();
				auto otherGroup = mBody.getOtherCommunityIdIndex();
				TransactionBodyMessage message;
				message["memos"_f].reserve(mBody.getMemos().size());
				for (auto& encryptedMemo : mBody.getMemos()) {
					message["memos"_f].push_back(EncryptedMemoMessage(
						encryptedMemo.getKeyType(),
						encryptedMemo.getMemo().copyAsVector()
					));
				}
				message["created_at"_f] = TimestampMessage{ createdAt.getSeconds(), createdAt.getNanos() };
				message["version_number"_f] = mBody.getVersionNumber();
				message["type"_f] = mBody.getType();
				if (otherGroup.has_value()) {
					auto communityIdOptional = g_appContext->getCommunityIds().getDataForIndex(otherGroup.value());
					if (!communityIdOptional.has_value()) {
						throw DictionaryMissingEntryException("missing entry by serializing TransactionBody", "other_group");
					}
					message["other_group"_f] = communityIdOptional.value();
				}

				if (mBody.isCommunityRoot())
				{
					auto communityRoot = mBody.getCommunityRoot();
					if (!communityRoot) {
						throw MissingMemberException("missing member by serializing TransactionBody", "CommunityRoot");
					}
					// span<byte>{ reinterpret_cast<byte*>(bodyBytes.data()), bodyBytes.size() };
					auto communityRootValue = communityRoot.value();
					auto publicKey = communityRootValue.publicKeyIndex.getRawKey();
					auto gmwPublicKey = communityRootValue.gmwPublicKeyIndex.getRawKey();
					auto aufPublicKey = communityRootValue.aufPublicKeyIndex.getRawKey();

					message["community_root"_f] = CommunityRootMessage{
						publicKey.copyAsVector(),
						gmwPublicKey.copyAsVector(),
						aufPublicKey.copyAsVector()
					};
				} 
				else if (mBody.isRegisterAddress())
				{
					auto registerAddress = mBody.getRegisterAddress().value();
					auto userPubkey = g_appContext->getPublicKey(registerAddress.userPublicKeyIndex);
					auto nameHash = g_appContext->getUseNameHashs().getDataForIndex(registerAddress.nameHashIndex);
					auto accountPubkey = g_appContext->getPublicKey(registerAddress.accountPublicKeyIndex);

					if (!userPubkey || !nameHash || !accountPubkey) {
						throw MissingMemberException("missing member of register address transaction", "userPublicKey||accountPublicKey||nameHash");
					}
					
					message["register_address"_f] = RegisterAddressMessage{
						userPubkey->copyAsVector(),
						registerAddress.addressType,
						nameHash->copyAsVector(),
						accountPubkey->copyAsVector(),
						registerAddress.derivationIndex
					};
					
					/*registerAddressMessage["user_pubkey"_f] = userPubkey->copyAsVector();
					registerAddressMessage["address_type"_f] = registerAddress.addressType;
					registerAddressMessage["name_hash"_f] = nameHash->copyAsVector();
					registerAddressMessage["account_pubkey"_f] = accountPubkey->copyAsVector();
					registerAddressMessage["derivation_index"_f] = registerAddress.derivationIndex;
					message["register_address"_f] = registerAddressMessage;
					*/
				}
				else if (mBody.isCreation())
				{
					auto creation = mBody.getCreation();
					auto& amount = creation->getRecipient();

					if (!amount.getPublicKey()) {
						throw MissingMemberException("missing member by serializing Gradido Creation Transaction", "recipient.pubkey");
					}
						
					message["creation"_f] = GradidoCreationMessage{ 
						createTransferAmountMessage(amount),
						TimestampSecondsMessage(creation->getTargetDate().getSeconds())
					};
				}
				else if (mBody.isTransfer()) 
				{
					auto transfer = mBody.getTransfer();
					auto& amount = transfer->getSender();
					if (!amount.getPublicKey()) {
						throw MissingMemberException("missing member by serializing Gradido Transfer Transaction", "sender.pubkey");
					}
					if (!transfer->getRecipient()) {
						throw MissingMemberException("missing member by serializing Gradido Transfer Transaction", "recipient");
					}
					message["transfer"_f] = GradidoTransferMessage{
						createTransferAmountMessage(amount),
						transfer->getRecipient()->copyAsVector()
					};
				}
				else if (mBody.isDeferredTransfer()) 
				{
					auto deferredTransfer = mBody.getDeferredTransfer();
					auto& transfer = mBody.getDeferredTransfer()->getTransfer();
					auto& amount = transfer.getSender();
					if (!amount.getPublicKey()) {
						throw MissingMemberException("missing member by serializing Gradido Deferred Transfer Transaction", "transfer.sender.pubkey");
					}
					if (!transfer.getRecipient()) {
						throw MissingMemberException("missing member by serializing Gradido Deferred Transfer Transaction", "transfer.recipient");
					}
					message["deferred_transfer"_f] = GradidoDeferredTransferMessage{
						GradidoTransferMessage{
							createTransferAmountMessage(amount),
							transfer.getRecipient()->copyAsVector()
						}, DurationSecondsMessage(deferredTransfer->getTimeoutDuration().getSeconds())
					};
				}
				else if (mBody.isRedeemDeferredTransfer()) 
				{
					auto redeemDeferredTransfer = mBody.getRedeemDeferredTransfer();
					auto& transfer = mBody.getRedeemDeferredTransfer()->getTransfer();
					auto& amount = transfer.getSender();
					if (!amount.getPublicKey()) {
						throw MissingMemberException("missing member by serializing Gradido Redeem Deferred Transfer Transaction", "transfer.sender.pubkey");
					}
					if (!transfer.getRecipient()) {
						throw MissingMemberException("missing member by serializing Gradido Redeem Deferred Transfer Transaction", "transfer.recipient");
					}
					message["redeem_deferred_transfer"_f] = GradidoRedeemDeferredTransferMessage{
						redeemDeferredTransfer->getDeferredTransferTransactionNr(),
						GradidoTransferMessage{
							createTransferAmountMessage(amount),
							transfer.getRecipient()->copyAsVector()
						}
					};
				}
				else if (mBody.isTimeoutDeferredTransfer()) 
				{
					auto timeoutDeferredTransfer = mBody.getTimeoutDeferredTransfer();
					
					message["timeout_deferred_transfer"_f] = GradidoTimeoutDeferredTransferMessage{
						timeoutDeferredTransfer->getDeferredTransferTransactionNr()
					};
				}
				else if (mBody.isCommunityFriendsUpdate()) 
				{
					message["community_friends_update"_f] = CommunityFriendsUpdateMessage{
						mBody.getCommunityFriendsUpdate()->getColorFusion()
					};
				}
				return message;
			}

			TransferAmountMessage TransactionBodyRole::createTransferAmountMessage(const data::TransferAmount& amount) const
			{
				std::string coinCommunityId;
				if (amount.getCoinCommunityIdIndex() != mBody.getCommunityIdIndex()) {
					auto coinCommunityIdOptional = g_appContext->getCommunityIds().getDataForIndex(amount.getCoinCommunityIdIndex());
					if (!coinCommunityIdOptional.has_value()) {
						throw DictionaryMissingEntryException(
							"missing community id for index in serialize::TransactionBodyRole::createTransferAmountMessage", 
							std::to_string(amount.getCoinCommunityIdIndex())
						);
					}
					coinCommunityId = g_appContext->getCommunityIds().getDataForIndex(amount.getCoinCommunityIdIndex()).value();
				}
				return TransferAmountMessage{
					amount.getPublicKey()->copyAsVector(),
					amount.getAmount().getGradidoCent(),
					coinCommunityId
				};
			}

			size_t TransactionBodyRole::calculateSerializedSize() const
			{
				auto otherGroupIdIndex = mBody.getOtherCommunityIdIndex();
				size_t otherGroupStringSize = 0;
				if (otherGroupIdIndex.has_value()) {
					auto otherGroupStringOptional = g_appContext->getCommunityIds().getDataForIndex(otherGroupIdIndex.value());
					if (!otherGroupStringOptional.has_value()) {
						throw MissingMemberException("cannot get community id for index", "otherGroup");
					}
					otherGroupStringSize = otherGroupStringOptional.value().size();
				}
										// timestamp						  // enum
				auto size = 12 + mBody.getVersionNumber().size() + 1 + otherGroupStringSize + 3;
				for (auto& encryptedMemo : mBody.getMemos()) {
					size += 8 + encryptedMemo.getMemo().size();
				}
				//printf("body base size: %lld\n", size);

				if (mBody.isCommunityRoot()) {
					// 3x pubkey
					size += 8 + 32 * 3;
					// printf("calculated size for community root: %lld\n", size);
				}
				else if (mBody.isRegisterAddress()) {
					auto registerAddress = mBody.getRegisterAddress();
					size += 4 + 32 * 3;
					size += 2 + 4;
					// printf("calculated size for register address: %lld\n", size);
				}
				else if (mBody.isCreation()) {
					auto creation = mBody.getCreation();
					size += 6 + calculateTransferAmountSerializedSize(creation->getRecipient());
					// printf("calculated size for gradido creation: %lld\n", size);
				}
				else if (mBody.isTransfer()) {
					auto transfer = mBody.getTransfer();
					if (!transfer->getRecipient()) {
						throw MissingMemberException("missing member by serializing Gradido Transfer Transaction", "recipient");
					}
					size += transfer->getRecipient()->size() + calculateTransferAmountSerializedSize(transfer->getSender());
					// printf("calculated size for gradido transfer: %lld\n", size);
				}
				else if (mBody.isDeferredTransfer()) {
					auto deferredTransfer = mBody.getDeferredTransfer();
					auto& transfer = deferredTransfer->getTransfer();
					if (!transfer.getRecipient()) {
						throw MissingMemberException("missing member by serializing Gradido Deferred Transfer Transaction", "transfer.recipient");
					}
					size += transfer.getRecipient()->size() + calculateTransferAmountSerializedSize(transfer.getSender()) + 8;
					// printf("calculated size for gradido deferred transfer: %lld\n", size);
				}
				else if (mBody.isRedeemDeferredTransfer()) {
					auto redeemDeferredTransfer = mBody.getRedeemDeferredTransfer();
					auto& transfer = redeemDeferredTransfer->getTransfer();
					if (!transfer.getRecipient()) {
						throw MissingMemberException("missing member by serializing Gradido Deferred Transfer Transaction", "transfer.recipient");
					}
					size += transfer.getRecipient()->size() + calculateTransferAmountSerializedSize(transfer.getSender()) + 8 + 8;
					// printf("calculated size for gradido redeem deferred transfer: %lld\n", size);
				}
				else if (mBody.isCommunityFriendsUpdate()) {
					size += 4;
					// printf("calculated size for community friends update: %lld\n", size);
				}
				return size;
			}

			size_t TransactionBodyRole::calculateTransferAmountSerializedSize(const data::TransferAmount& amount) const
			{
				if (!amount.getPublicKey()) {
					throw MissingMemberException("missing member by serializing TransferAmount", "amount.pubkey");
				}
				size_t coinCommunityStringSize = 0;
				if (amount.getCoinCommunityIdIndex() != mBody.getCommunityIdIndex()) {
					auto communityIdOptional = g_appContext->getCommunityIds().getDataForIndex(amount.getCoinCommunityIdIndex());
					if (!communityIdOptional.has_value()) {
						throw DictionaryMissingEntryException(
							"missing community id for index in serialize::TransactionBodyRole::createTransferAmountMessage",
							std::to_string(amount.getCoinCommunityIdIndex())
						);
					}
					coinCommunityStringSize = communityIdOptional.value().size();
				}
				return amount.getPublicKey()->size() + 8 + coinCommunityStringSize + 12;
			}
				
		}
	}
}