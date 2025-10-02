#include "gtest/gtest.h"
#include "gradido_blockchain/blockchain/Filter.h"
#include "gradido_blockchain/blockchain/SearchDirection.h"
#include "gradido_blockchain/serialization/toJson.h"
#include "gradido_blockchain/serialization/toJsonString.h"

using namespace gradido::blockchain;
using namespace gradido::data;
using namespace std::chrono;
using namespace rapidjson;

TEST(ToJsonTest, Filter) {
	auto startDate = system_clock::now();
	auto endDate = system_clock::now() + days(10);

	Filter filter;
	filter.minTransactionNr = 1;
	filter.maxTransactionNr = 2;
	filter.involvedPublicKey = std::make_shared<memory::Block>(memory::Block::fromHex(std::string_view("1234567890abcdef")));
	filter.searchDirection = SearchDirection::ASC;
	filter.pagination = Pagination{1, 2};
	filter.coinCommunityId = "fedcba9876543210";
	filter.timepointInterval = TimepointInterval(startDate, endDate);
	filter.transactionType = TransactionType::COMMUNITY_ROOT;
	filter.filterFunction = [](const TransactionEntry&) -> FilterResult { 
		return FilterResult::STOP;
	};	
  
	// auto jsonString = serialization::toJsonString(filter, true);
	// printf("%s\n", jsonString.data());

	// should print something like this: 
	/*
	* 
	{
		"minTransactionNr": 1,
		"maxTransactionNr": 2,
		"involvedPublicKey": "1234567890abcdef",
		"searchDirection": "ASC",
		"pagination": {
			"size": 1,
			"page": 2
		},
		"coinCommunityId": "fedcba9876543210",
		"timepointInterval": {
			"start": "2025-10-02T08:40:45Z",
			"end": "2025-10-12T08:40:45Z"
		},
		"transactionType": "COMMUNITY_ROOT",
		"filterFunction": true
	}
	*/

	Document base;
	auto json = serialization::toJson(filter, base.GetAllocator());

	EXPECT_EQ(1, json["minTransactionNr"].GetInt());
	EXPECT_EQ(2, json["maxTransactionNr"].GetInt());
	EXPECT_STREQ("1234567890abcdef", json["involvedPublicKey"].GetString());
	EXPECT_STREQ("ASC", json["searchDirection"].GetString());
	EXPECT_EQ(1, json["pagination"]["size"].GetInt());
	EXPECT_EQ(2, json["pagination"]["page"].GetInt());
	EXPECT_STREQ("fedcba9876543210", json["coinCommunityId"].GetString());
	// EXPECT_EQ(startDate.time_since_epoch().count(), json["timepointInterval"]["start"]["value"].GetInt64());
	// EXPECT_EQ(endDate.time_since_epoch().count() + 1, json["timepointInterval"]["end"]["value"].GetInt64());
	EXPECT_STREQ("COMMUNITY_ROOT", json["transactionType"].GetString());
	EXPECT_TRUE(json["filterFunction"].GetBool());
}