#include "gradido_blockchain/v3_3/interaction/deserialize/TransactionBodyRole.h"
#include "gradido_blockchain/v3_3/interaction/deserialize/TimestampRole.h"
#include "gradido_blockchain/v3_3/interaction/deserialize/TimestampSecondsRole.h"
#include "gradido_blockchain/v3_3/interaction/deserialize/TransferAmountRole.h"
#include "gradido_blockchain/v3_3/interaction/deserialize/RegisterAddressRole.h"
#include "gradido_blockchain/v3_3/interaction/deserialize/GradidoTransferRole.h"
#include "gradido_blockchain/v3_3/interaction/deserialize/Exceptions.h"

namespace gradido {
	namespace v3_3 {
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
					mBody = std::make_shared<data::TransactionBody>(
						bodyMessage["memo"_f].value_or(""),
						TimestampRole(bodyMessage["created_at"_f].value()).data(),
						bodyMessage["version_number"_f].value(),
						bodyMessage["type"_f].value(),
						bodyMessage["other_group"_f].value_or("")
					);
					if (bodyMessage["transfer"_f].has_value()) {
						auto transferMessage = bodyMessage["transfer"_f].value();
						if (!bodyMessage["transfer"_f].value()["sender"_f].has_value()) {
							throw MissingMemberException("missing member on deserialize transaction body transfer transaction", "transfer.sender");
						}
						mBody->transfer = std::make_shared<GradidoTransfer>(
							GradidoTransferRole(bodyMessage["transfer"_f].value()).data()
						);
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

						mBody->creation = std::make_shared<GradidoCreation>(
							TransferAmountRole(creationMessage["recipient"_f].value()).data(),
							TimestampSecondsRole(creationMessage["target_date"_f].value()).data()
						);
					}
					else if (bodyMessage["community_friends_update"_f].has_value()) {
						auto communityFriendsUpdateMessage = bodyMessage["community_friends_update"_f].value();
						if (!communityFriendsUpdateMessage["color_fusion"_f].has_value()) {
							throw MissingMemberException("missing member on deserialize community friends update transaction", "color_fusion");
						}
						mBody->communityFriendsUpdate = std::make_shared<CommunityFriendsUpdate>(
							communityFriendsUpdateMessage["color_fusion"_f].value()
						);
					}
					else if (bodyMessage["register_address"_f].has_value()) {
						mBody->registerAddress = std::make_shared<RegisterAddress>(
							RegisterAddressRole(bodyMessage["register_address"_f].value())
						);
					}
					else if (bodyMessage["deferred_transfer"_f].has_value()) {
						auto deferredTransferMessage = bodyMessage["deferred_transfer"_f].value();
						const char* exceptionMessage = "missing member on deserialize deferred transfer transaction";
						if (!deferredTransferMessage["transfer"_f].has_value()) {
							throw MissingMemberException(exceptionMessage, "transfer");
						}
						if (!deferredTransferMessage["timeout"_f].has_value()) {
							throw MissingMemberException(exceptionMessage, "timeout");
						}
						mBody->deferredTransfer = std::make_shared<GradidoDeferredTransfer>(
							GradidoTransferRole(deferredTransferMessage["transfer"_f].value()).data(),
							TimestampSecondsRole(deferredTransferMessage["timeout"_f].value()).data()
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
						mBody->communityRoot = std::make_shared<CommunityRoot>(
							std::make_shared<memory::Block>(communityRootMessage["pubkey"_f].value()),
							std::make_shared<memory::Block>(communityRootMessage["gmw_pubkey"_f].value()),
							std::make_shared<memory::Block>(communityRootMessage["auf_pubkey"_f].value())
						);
					}
				}
				
			}
		}
	}
}