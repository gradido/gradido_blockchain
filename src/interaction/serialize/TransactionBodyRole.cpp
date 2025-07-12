#include "gradido_blockchain/data/TransactionBody.h"
#include "gradido_blockchain/interaction/serialize/TransactionBodyRole.h"
#include "gradido_blockchain/interaction/serialize/Exceptions.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"

namespace gradido {
	using namespace data;
	using namespace std;
	namespace interaction {
		namespace serialize {

			TransactionBodyMessage TransactionBodyRole::getMessage() const
			{
				auto createdAt = mBody.getCreatedAt();
				auto otherGroup = mBody.getOtherGroup();
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
				if (otherGroup.size()) {
					message["other_group"_f] = otherGroup;
				}

				if (mBody.isCommunityRoot()) 
				{
					auto communityRoot = mBody.getCommunityRoot();
					if (!communityRoot->getPublicKey()) {
						throw MissingMemberException("missing member by serializing CommunityRoot Transaction", "pubkey");
					}
					if (!communityRoot->getGmwPubkey()) {
						throw MissingMemberException("missing member by serializing CommunityRoot Transaction", "gmwPubkey");
					}
					if (!communityRoot->getAufPubkey()) {
						throw MissingMemberException("missing member by serializing CommunityRoot Transaction", "aufPubkey");
					}

					message["community_root"_f] = CommunityRootMessage {
						communityRoot->getPublicKey()->copyAsVector(),
						communityRoot->getGmwPubkey()->copyAsVector(),
						communityRoot->getAufPubkey()->copyAsVector()
					};
				} 
				else if (mBody.isRegisterAddress())
				{
					auto registerAddress = mBody.getRegisterAddress();
					auto userPubkey = registerAddress->getUserPublicKey();
					auto nameHash = registerAddress->getNameHash();
					auto accountPubkey = registerAddress->getAccountPublicKey();

					RegisterAddressMessage registerAddressMessage;
					if (userPubkey) {
						registerAddressMessage["user_pubkey"_f] = userPubkey->copyAsVector();
					}
					registerAddressMessage["address_type"_f] = registerAddress->getAddressType();
					if (nameHash) {
						registerAddressMessage["name_hash"_f] = nameHash->copyAsVector();
					}
					if (accountPubkey) {
						registerAddressMessage["account_pubkey"_f] = accountPubkey->copyAsVector();
					}
					registerAddressMessage["derivation_index"_f] = registerAddress->getDerivationIndex();
					message["register_address"_f] = registerAddressMessage;
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
				return TransferAmountMessage{
					amount.getPublicKey()->copyAsVector(),
					amount.getAmount().getGradidoCent(),
					amount.getCommunityId()
				};
			}

			size_t TransactionBodyRole::calculateSerializedSize() const
			{
										// timestamp						  // enum
				auto size = 12 + mBody.getVersionNumber().size() + 1 + mBody.getOtherGroup().size() + 3;
				for (auto& encryptedMemo : mBody.getMemos()) {
					size += 8 + encryptedMemo.getMemo().size();
				}
				//printf("body base size: %lld\n", size);

				if (mBody.isCommunityRoot()) {
					auto communityRoot = mBody.getCommunityRoot();
					if (!communityRoot->getPublicKey()) {
						throw MissingMemberException("missing member by serializing CommunityRoot Transaction", "pubkey");
					}
					if (!communityRoot->getGmwPubkey()) {
						throw MissingMemberException("missing member by serializing CommunityRoot Transaction", "gmwPubkey");
					}
					if (!communityRoot->getAufPubkey()) {
						throw MissingMemberException("missing member by serializing CommunityRoot Transaction", "aufPubkey");
					}
					// 3x pubkey
					size += 8
						+ communityRoot->getPublicKey()->size()
						+ communityRoot->getGmwPubkey()->size()
						+ communityRoot->getAufPubkey()->size();
					// printf("calculated size for community root: %lld\n", size);
				}
				else if (mBody.isRegisterAddress()) {
					auto registerAddress = mBody.getRegisterAddress();
					size += 4;
					if (registerAddress->getUserPublicKey()) size += registerAddress->getUserPublicKey()->size();
					if (registerAddress->getNameHash()) size += registerAddress->getNameHash()->size();
					if (registerAddress->getAccountPublicKey()) size += registerAddress->getAccountPublicKey()->size();
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
				return amount.getPublicKey()->size() + 8 + amount.getCommunityId().size() + 12;
			}
				
		}
	}
}