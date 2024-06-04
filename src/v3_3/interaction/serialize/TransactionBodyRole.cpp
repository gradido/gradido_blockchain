#include "gradido_blockchain/v3_3/interaction/serialize/TransactionBodyRole.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace serialize {

				memory::ConstBlockPtr TransactionBodyRole::run() const
				{
					/*
					auto& createdAt = mBody.createdAt;
					data::TransactionBodyMessage message;
					data::TimestampMessage created_at{ createdAt.seconds, createdAt.nanos };

					message["memo"_f] = mBody.memo;
					message["created_at"_f] = created_at;
					message["version_number"_f] = mBody.versionNumber;
					message["type"_f] = mBody.type;
					if (mBody.otherGroup.size()) {
						message["other_group"_f] = mBody.otherGroup;
					}
					
					if (mBody.isCommunityFriendsUpdate()) {
						data::CommunityFriendsUpdateMessage communityFriendsUpdate{ mBody.communityFriendsUpdate->colorFusion };
						message["community_friends_update"_f] = communityFriendsUpdate;
					}
					else if (mBody.isCommunityRoot()) {
						auto communityRoot = mBody.communityRoot;
						data::CommunityRootMessage communityRootMessage {
							communityRoot->pubkey,
							communityRoot->gmwPubkey,
							communityRoot->aufPubkey
						};
						message["community_root"_f] = communityRootMessage;
					}
					else if (mBody.isCreation()) {
						auto creation = mBody.creation;
						data::TimestampSecondsMessage targetDateMessage{ creation->targetDate.seconds };
						data::GradidoCreationMessage creationMessage{
							getTransferAmountMessage(creation->recipient),
							targetDateMessage
						};
						message["creation"_f] = creationMessage;
					}
					else if (mBody.isDeferredTransfer()) {
						auto deferredTransfer = mBody.deferredTransfer;
						data::TimestampSeconds timeout{ deferredTransfer->timeout };
						data::GradidoDeferredTransferMessage deferredTransferMessage {
							getTransferMessage(deferredTransfer->transfer),
							timeout
						};
						message["deferred_transfer"_f] = deferredTransferMessage;
					}
					else if (mBody.isTransfer()) {
						auto transfer = mBody.transfer;
						data::GradidoTransferMessage transferMessage{
							getTransferAmountMessage(transfer->sender),
							transfer->recipient
						};
						message["transfer"_f] = transferMessage;
					}
					else if (mBody.isRegisterAddress()) {
						message["register_address"_f] = getRegisterAddressMessage(*mBody.registerAddress);
					}
					else {
						assert(false || "none transaction type set");
					}

					return encode(message, 1);
					*/
					return make_shared<Block>(1);
				}

		/*		data::GradidoTransferMessage TransactionBodyRole::getTransferMessage(const data::GradidoTransfer& transfer) const {
					data::GradidoTransferMessage transferMessage{ getTransferAmountMessage(transfer.sender), transfer.recipient };
					return transferMessage;
				}

				data::TransferAmountMessage TransactionBodyRole::getTransferAmountMessage(const data::TransferAmount& amount) const 
				{
					data::TransferAmountMessage transferAmountMessage{
						amount.recipientPubkey,
						amount.amount.toString(),
						amount.communityId
					};

					//transferAmountMessage["pubkey"_f] = span<uint8_t, ED25519_PUBLIC_KEY_SIZE>(amount.recipientPubkey->data());
					//transferAmountMessage["amount"_f] = amount.amount.toString();
					if (!amount.communityId.size()) {
						transferAmountMessage["community_id"_f] = nullptr;
					}
					return transferAmountMessage;
				}

				data::RegisterAddressMessage TransactionBodyRole::getRegisterAddressMessage(const data::RegisterAddress& registerAddress) const
				{
					data::RegisterAddressMessage message{
						registerAddress.userPubkey,
						registerAddress.addressType,
						registerAddress.nameHash,
						registerAddress.accountPubkey,
						registerAddress.derivationIndex
					};
					/*
					if (registerAddress.userPubkey) {
						message["user_pubkey"_f] = *registerAddress.userPubkey;
					}
					message["address_type"_f] = registerAddress.addressType;
					if (registerAddress.nameHash) {
						message["name_hash"_f] = *registerAddress.nameHash;
					}
					if (registerAddress.accountPubkey) {
						message["account_pubkey"_f] = *registerAddress.accountPubkey;
					}
					message["derivation_index"_f] = registerAddress.derivationIndex;
					
					return message;
				}
				//*/
			}
		}
	}
}