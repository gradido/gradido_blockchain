#include "gradido_blockchain/v3_3/interaction/deserialize/TransactionBodyRole.h"
#include "gradido_blockchain/v3_3/interaction/deserialize/TimestampRole.h"
#include "gradido_blockchain/v3_3/interaction/deserialize/TimestampSecondsRole.h"
#include "gradido_blockchain/v3_3/interaction/deserialize/TransferAmountRole.h"
#include "gradido_blockchain/v3_3/interaction/deserialize/RegisterAddressRole.h"
#include "gradido_blockchain/v3_3/interaction/deserialize/GradidoTransferRole.h"

namespace gradido {
	namespace v3_3 {
		using namespace data;
		namespace interaction {
			namespace deserialize {

				TransactionBodyRole::TransactionBodyRole(const TransactionBodyMessage& bodyMessage)
				{
					mBody = std::make_unique<data::TransactionBody>(
						bodyMessage["memo"_f].value_or(""),
						TimestampRole(bodyMessage["created_at"_f].value()).data(),
						bodyMessage["version_number"_f].value(),
						bodyMessage["type"_f].value(),
						bodyMessage["other_group"_f].value_or("")
					);
					if (bodyMessage["transfer"_f].has_value()) {
						printf("transfer\n");
						auto transferMessage = bodyMessage["transfer"_f].value();
						mBody->transfer = std::make_shared<GradidoTransfer>(
							GradidoTransferRole(bodyMessage["transfer"_f].value()).data()
						);
					}
					else if (bodyMessage["creation"_f].has_value()) {
						printf("creation\n");
						auto creationMessage = bodyMessage["creation"_f].value();
						mBody->creation = std::make_shared<GradidoCreation>(
							TransferAmountRole(creationMessage["recipient"_f].value()).data(),
							TimestampSecondsRole(creationMessage["target_date"_f].value()).data()
						);
					}
					else if (bodyMessage["community_friends_update"_f].has_value()) {
						printf("community_friends_update\n");
						mBody->communityFriendsUpdate = std::make_shared<CommunityFriendsUpdate>(
							bodyMessage["community_friends_update"_f].value()["color_fusion"_f].value()
						);
					}
					else if (bodyMessage["register_address"_f].has_value()) {
						printf("register_address\n");
						mBody->registerAddress = std::make_shared<RegisterAddress>(
							RegisterAddressRole(bodyMessage["register_address"_f].value())
						);
					}
					else if (bodyMessage["deferred_transfer"_f].has_value()) {
						printf("deferred_transfer\n");
						mBody->deferredTransfer = std::make_shared<GradidoDeferredTransfer>(
							GradidoTransferRole(bodyMessage["deferred_transfer"_f].value()["transfer"_f].value()).data(),
							TimestampSecondsRole(bodyMessage["deferred_transfer"_f].value()["timeout"_f].value()).data()
						);
					}
					else if (bodyMessage["community_root"_f].has_value()) {
						printf("community root\n");
						auto communityRootMessage = bodyMessage["community_root"_f].value();
						if (communityRootMessage["pubkey"_f].has_value()) {
							printf("pubkey size: %lld\n", communityRootMessage["pubkey"_f].value().size());
						}
						if (!communityRootMessage["gmw_pubkey"_f].has_value()) {
							printf("gmw_pubkey missing\n");
						}
						if (!communityRootMessage["auf_pubkey"_f].has_value()) {
							printf("auf_pubkey missing\n");
						}
						assert(communityRootMessage["pubkey"_f].has_value());
						assert(communityRootMessage["gmw_pubkey"_f].has_value());
						assert(communityRootMessage["auf_pubkey"_f].has_value());
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