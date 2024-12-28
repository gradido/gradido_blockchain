#include "gradido_blockchain/data/TransactionBody.h"
#include "gradido_blockchain/data/EncryptedMemo.h"
#include "gradido_blockchain/interaction/deserialize/EncryptedMemoRole.h"
#include "gradido_blockchain/interaction/deserialize/TransactionBodyRole.h"
#include "gradido_blockchain/interaction/deserialize/TimestampRole.h"
#include "gradido_blockchain/interaction/deserialize/TimestampSecondsRole.h"
#include "gradido_blockchain/interaction/deserialize/TransferAmountRole.h"
#include "gradido_blockchain/interaction/deserialize/RegisterAddressRole.h"
#include "gradido_blockchain/interaction/deserialize/GradidoTransferRole.h"
#include "gradido_blockchain/interaction/deserialize/Exceptions.h"

using namespace std;

namespace gradido {
	using namespace data;
	namespace interaction {
		namespace deserialize {
		
			TransactionBodyRole::TransactionBodyRole(const TransactionBodyMessage& bodyMessage)
			{
				const char* rootExceptionMessage = "missing member on deserialize transaction body";
				
				if (!bodyMessage["created_at"_f].has_value()) {
					throw MissingMemberException(rootExceptionMessage, "created_at");
				}
				if (!bodyMessage["version_number"_f].has_value()) {
					throw MissingMemberException(rootExceptionMessage, "version_number");
				}
				if (!bodyMessage["type"_f].has_value()) {
					throw MissingMemberException(rootExceptionMessage, "type");
				}
				mTransactionBody = std::make_unique<TransactionBody>();
				/*auto memoMessages = bodyMessage["memos"_f];
				if (memoMessages.size()) {
					mTransactionBody->mMemos.reserve(memoMessages.size());
					for (int i = 0; i < memoMessages.size(); i++) {
						mTransactionBody->mMemos.push_back(EncryptedMemoRole(memoMessages[i]));
					}
				}
				*/
				mTransactionBody->mCreatedAt = TimestampRole(bodyMessage["created_at"_f].value()).data();
				mTransactionBody->mVersionNumber = bodyMessage["version_number"_f].value();
				mTransactionBody->mType = bodyMessage["type"_f].value();
				mTransactionBody->mOtherGroup = bodyMessage["other_group"_f].value_or("");

				if (bodyMessage["transfer"_f].has_value()) {
					auto transferMessage = bodyMessage["transfer"_f].value();
					if (!bodyMessage["transfer"_f].value()["sender"_f].has_value()) {
						throw MissingMemberException("missing member on deserialize transaction body transfer transaction", "transfer.sender");
					}
					mTransactionBody->mTransfer = std::move(GradidoTransferRole(bodyMessage["transfer"_f].value()).run());
				}
				else if (bodyMessage["creation"_f].has_value()) {
					auto creationMessage = bodyMessage["creation"_f].value();
					const char* exceptionMessage = "missing member on deserialize transaction body creation transaction";
					if (!creationMessage["recipient"_f].has_value()) {
						throw MissingMemberException(exceptionMessage, "recipient");
					}
					if (!creationMessage["target_date"_f].has_value()) {
						throw MissingMemberException(exceptionMessage, "target_date");
					}
					mTransactionBody->mCreation = make_shared<data::GradidoCreation>(
						TransferAmountRole(creationMessage["recipient"_f].value()).data(),
						TimestampSecondsRole(creationMessage["target_date"_f].value()).data()
					);
				}
				else if (bodyMessage["community_friends_update"_f].has_value()) {
					auto communityFriendsUpdateMessage = bodyMessage["community_friends_update"_f].value();
					if (!communityFriendsUpdateMessage["color_fusion"_f].has_value()) {
						throw MissingMemberException("missing member on deserialize community friends update transaction", "color_fusion");
					}
					mTransactionBody->mCommunityFriendsUpdate = make_shared<data::CommunityFriendsUpdate>(
						communityFriendsUpdateMessage["color_fusion"_f].value()
					);
				}
				else if (bodyMessage["register_address"_f].has_value()) {
					mTransactionBody->mRegisterAddress = std::move(
						RegisterAddressRole(bodyMessage["register_address"_f].value()).run()
					);
				}
				else if (bodyMessage["deferred_transfer"_f].has_value()) {
					auto deferredTransferMessage = bodyMessage["deferred_transfer"_f].value();
					const char* exceptionMessage = "missing member on deserialize deferred transfer transaction";
					if (!deferredTransferMessage["transfer"_f].has_value()) {
						throw MissingMemberException(exceptionMessage, "transfer");
					}
					if (!deferredTransferMessage["timeout_duration"_f].has_value()) {
						throw MissingMemberException(exceptionMessage, "timeout_duration");
					}
					mTransactionBody->mDeferredTransfer = make_shared<data::GradidoDeferredTransfer>(
						*GradidoTransferRole(deferredTransferMessage["transfer"_f].value()).run().get(),
						deferredTransferMessage["timeout_duration"_f].value()
					);
				}
				else if (bodyMessage["redeem_deferred_transfer"_f].has_value()) {
					auto redeemDeferredTransferMessage = bodyMessage["redeem_deferred_transfer"_f].value();
					const char* exceptionMessage = "missing member on deserialize redeem deferred transfer transaction";
					if (!redeemDeferredTransferMessage["deferred_tansfer_transaction_nr"_f].has_value()) {
						throw MissingMemberException(exceptionMessage, "deferredTransferTransactionNr");
					}
					if (!redeemDeferredTransferMessage["transfer"_f].has_value()) {
						throw MissingMemberException(exceptionMessage, "transfer");
					}
					mTransactionBody->mRedeemDeferredTransfer = make_shared<data::GradidoRedeemDeferredTransfer>(
						redeemDeferredTransferMessage["deferred_tansfer_transaction_nr"_f].value(),
						*GradidoTransferRole(redeemDeferredTransferMessage["transfer"_f].value()).run().get()
					);
				}
				else if (bodyMessage["timeout_deferred_transfer"_f].has_value()) {
					auto timeoutDeferredTransferMessage = bodyMessage["timeout_deferred_transfer"_f].value();
					const char* exceptionMessage = "missing member on deserialize timeout deferred transfer transaction";
					if (!timeoutDeferredTransferMessage["deferred_tansfer_transaction_nr"_f].has_value()) {
						throw MissingMemberException(exceptionMessage, "deferredTransferTransactionNr");
					}
					mTransactionBody->mTimeoutDeferredTransfer = make_shared<data::GradidoTimeoutDeferredTransfer>(
						timeoutDeferredTransferMessage["deferred_tansfer_transaction_nr"_f].value()
					);
				}
				else if (bodyMessage["community_root"_f].has_value()) {
					auto communityRootMessage = bodyMessage["community_root"_f].value();
					const char* exceptionMessage = "missing member on deserialize community root transaction";
					if (!communityRootMessage["pubkey"_f].has_value()) {
						throw MissingMemberException(exceptionMessage, "pubkey");
					}
					if (!communityRootMessage["gmw_pubkey"_f].has_value()) {
						throw MissingMemberException(exceptionMessage, "gmw_pubkey");
					}
					if (!communityRootMessage["auf_pubkey"_f].has_value()) {
						throw MissingMemberException(exceptionMessage, "auf_pubkey");
					}
					mTransactionBody->mCommunityRoot = make_shared<data::CommunityRoot>(
						std::make_shared<memory::Block>(communityRootMessage["pubkey"_f].value()),
						std::make_shared<memory::Block>(communityRootMessage["gmw_pubkey"_f].value()),
						std::make_shared<memory::Block>(communityRootMessage["auf_pubkey"_f].value())
					);
				}
			}

			TransactionBodyRole::~TransactionBodyRole()
			{

			}
		}
	}
}