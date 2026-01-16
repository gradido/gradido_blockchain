#include "gradido_blockchain/export.h"
#include "gradido_blockchain/data/hiero/AccountId.h"
#include "gradido_blockchain/data/hiero/TopicId.h"
#include "gradido_blockchain/data/hiero/TransactionId.h"
#include "gradido_blockchain/serialization/toJson.h"

#include "rapidjson/document.h"

using rapidjson::Document;
using hiero::TransactionId, hiero::AccountId, hiero::TopicId;

namespace serialization {
	DEFINE_TO_JSON(AccountId, {
		obj.AddMember("shardNum", value.getShardNum(), alloc);
		obj.AddMember("realmNum", value.getRealmNum(), alloc);
		if (value.getAccountNum()) {
			obj.AddMember("accountNum", value.getAccountNum(), alloc);
		}
		else {
			obj.AddMember("alias", toJson(value.getAlias(), alloc), alloc);
		}
		
	})

	DEFINE_TO_JSON(TransactionId, {
		obj.AddMember("transactionValidStart", toJson(value.getTransactionValidStart(), alloc), alloc);
		obj.AddMember("accountId", toJson(value.getAccountId(), alloc), alloc);
		if (value.isScheduled()) {
			obj.AddMember("scheduled", "true", alloc);
		}
		if (value.getNonce()) {
			obj.AddMember("nonce", value.getNonce(), alloc);
		}
	})

	DEFINE_TO_JSON(TopicId, {
		obj.AddMember("shard", value.getShardNum(), alloc);
		obj.AddMember("realm", value.getRealmNum(), alloc);
		obj.AddMember("topic", value.getTopicNum(), alloc);
	})
}