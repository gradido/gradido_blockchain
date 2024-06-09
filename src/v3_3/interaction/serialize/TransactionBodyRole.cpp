#include "gradido_blockchain/v3_3/interaction/serialize/TransactionBodyRole.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"

namespace gradido {
	namespace v3_3 {
		using namespace data;
		namespace interaction {
			namespace serialize {

				TransactionBodyMessage TransactionBodyRole::getMessage() const
				{
					TransactionBodyMessage message;
					message["memo"_f] = mBody.memo;
					message["created_at"_f] = TimestampMessage{
						mBody.createdAt.seconds,
						mBody.createdAt.nanos
					};
					message["version_number"_f] = mBody.versionNumber;
					message["type"_f] = mBody.type;
					if (mBody.otherGroup.size()) {
						message["other_group"_f] = mBody.otherGroup;
					}

					if (mBody.isCommunityRoot()) 
					{
						auto communityRoot = mBody.communityRoot;
						message["community_root"_f] = CommunityRootMessage {
							communityRoot->pubkey->copyAsVector(),
							communityRoot->gmwPubkey->copyAsVector(),
							communityRoot->aufPubkey->copyAsVector()
						};
					} 
					else if (mBody.isRegisterAddress()) 
					{
						auto registerAddress = mBody.registerAddress;
						RegisterAddressMessage registerAddressMessage;
						if (registerAddress->userPubkey) {
							registerAddressMessage["user_pubkey"_f] = registerAddress->userPubkey->copyAsVector();
						}
						registerAddressMessage["address_type"_f] = registerAddress->addressType;
						if (registerAddress->nameHash) {
							registerAddressMessage["name_hash"_f] = registerAddress->nameHash->copyAsVector();
						}
						if (registerAddress->accountPubkey) {
							registerAddressMessage["account_pubkey"_f] = registerAddress->accountPubkey->copyAsVector();
						}
						registerAddressMessage["derivation_index"_f] = registerAddress->derivationIndex;
						message["register_address"_f] = registerAddressMessage;
					}
					else if (mBody.isCreation())
					{
						auto creation = mBody.creation;
						auto& amount = creation->recipient;
						TimestampSecondsMessage targetDateMessage{ creation->targetDate.seconds };
						
						message["creation"_f] = GradidoCreationMessage{
							TransferAmountMessage {
								amount.pubkey->copyAsVector(),
								amount.amount.toString(),
								amount.communityId
							},  TimestampSecondsMessage { creation->targetDate.seconds }
						};
					}
					else if (mBody.isTransfer()) {
						auto transfer = mBody.transfer;
						auto& amount = transfer->sender;
						message["transfer"_f] = GradidoTransferMessage{
							TransferAmountMessage {
								amount.pubkey->copyAsVector(),
								amount.amount.toString(),
								amount.communityId
							}, transfer->recipient->copyAsVector()
						};
					}
					else if (mBody.isDeferredTransfer()) {
						auto deferredTransfer = mBody.deferredTransfer;
						auto& transfer = mBody.deferredTransfer->transfer;
						auto& amount = transfer.sender;
						message["deferred_transfer"_f] = GradidoDeferredTransferMessage{
							GradidoTransferMessage{
								TransferAmountMessage {
									amount.pubkey->copyAsVector(),
									amount.amount.toString(),
									amount.communityId
								}, transfer.recipient->copyAsVector()
							}, TimestampSecondsMessage{deferredTransfer->timeout.seconds}
						};
					}
					else if (mBody.isCommunityFriendsUpdate()) {
						message["community_friends_update"_f] = CommunityFriendsUpdateMessage{
							mBody.communityFriendsUpdate->colorFusion
						};
					}
					return message;
				}

				size_t TransactionBodyRole::calculateSerializedSize() const
				{
											// timestamp						  // enum
					auto size = mBody.memo.size() + 12 + mBody.versionNumber.size() + 1 + mBody.otherGroup.size() + 3;
					//printf("body base size: %lld\n", size);

					if (mBody.isCommunityRoot()) {
						auto communityRoot = mBody.communityRoot;
						// 3x pubkey
						size += 8
							+ communityRoot->pubkey->size()
							+ communityRoot->gmwPubkey->size() 
							+ communityRoot->aufPubkey->size();
						// printf("calculated size for community root: %lld\n", size);
					}
					else if (mBody.isRegisterAddress()) {
						auto registerAddress = mBody.registerAddress;
						size += 4;
						if (registerAddress->userPubkey) size += registerAddress->userPubkey->size();
						if (registerAddress->nameHash) size += registerAddress->nameHash->size();
						if (registerAddress->accountPubkey) size += registerAddress->accountPubkey->size();
						size += 2 + 4;
						// printf("calculated size for register address: %lld\n", size);
					}
					else if (mBody.isCreation()) {
						auto creation = mBody.creation;
						size += 6 + calculateTransferAmountSerializedSize(creation->recipient);
						// printf("calculated size for gradido creation: %lld\n", size);
					}
					else if (mBody.isTransfer()) {
						auto transfer = mBody.transfer;
						size += transfer->recipient->size() + calculateTransferAmountSerializedSize(transfer->sender);
						// printf("calculated size for gradido transfer: %lld\n", size);
					}
					else if (mBody.isDeferredTransfer()) {
						auto deferredTransfer = mBody.deferredTransfer;
						auto& transfer = deferredTransfer->transfer;
						size += transfer.recipient->size() + calculateTransferAmountSerializedSize(transfer.sender) + 8;
						// printf("calculated size for gradido deferred transfer: %lld\n", size);
					}
					else if (mBody.isCommunityFriendsUpdate()) {
						size += 4;
						// printf("calculated size for community friends update: %lld\n", size);
					}
					return size;
				}

				size_t TransactionBodyRole::calculateTransferAmountSerializedSize(const data::TransferAmount& amount) const
				{
					return amount.pubkey->size() + amount.amount.toString().size() + amount.communityId.size() + 12;
				}
				
			}
		}
	}
}