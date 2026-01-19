#include "gradido_blockchain/blockchain/Filter.h"
#include "gradido_blockchain/blockchain/Pagination.h"
#include "gradido_blockchain/export.h"
#include "gradido_blockchain/serialization/toJson.h"

using namespace rapidjson;
using namespace gradido::blockchain;

namespace serialization {
	DEFINE_TO_JSON(Pagination, {
		obj.AddMember("size", value.size, alloc);
		obj.AddMember("page", value.page, alloc);
	})
	
	DEFINE_TO_JSON(TimepointInterval, {
		obj.AddMember("start", toJson(value.getStartDate(), alloc), alloc);
		obj.AddMember("end", toJson(value.getEndDate(), alloc), alloc);
	})
	
	DEFINE_TO_JSON(Filter, {
		obj.AddMember("minTransactionNr", value.minTransactionNr, alloc);
		obj.AddMember("maxTransactionNr", value.maxTransactionNr, alloc);
		obj.AddMember("involvedPubkey", toJson(value.involvedPublicKey, alloc), alloc);
		obj.AddMember("updatedBalancePublicKey", toJson(value.updatedBalancePublicKey, alloc), alloc);
		obj.AddMember("searchDirection", toJson(value.searchDirection, alloc), alloc);
		obj.AddMember("pagination", toJson(value.pagination, alloc), alloc);
		if (value.coinCommunityIdIndex.has_value()) {
			obj.AddMember("coinCommunityIdIndex", value.coinCommunityIdIndex.value(), alloc);
		}
		obj.AddMember("timepointInterval", toJson(value.timepointInterval, alloc), alloc);
		obj.AddMember("transactionType", toJson(value.transactionType, alloc), alloc);
		obj.AddMember("filterFunction", value.filterFunction != nullptr, alloc);
	})
}

