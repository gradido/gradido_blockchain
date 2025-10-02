#include "gradido_blockchain/blockchain/Filter.h"
#include "gradido_blockchain/blockchain/Pagination.h"
#include "gradido_blockchain/export.h"
#include "gradido_blockchain/serialization/toJson.h"

using namespace rapidjson;
using namespace gradido::blockchain;

namespace serialization {

	template<>
	Value toJson(const Pagination& pagination, Document::AllocatorType& alloc)
	{
		Value obj(kObjectType);
		obj.AddMember("size", pagination.size, alloc);
		obj.AddMember("page", pagination.page, alloc);
		return obj;
	}	

	template<>
	Value toJson(const TimepointInterval& timepointInterval, Document::AllocatorType& alloc)
	{
		Value obj(rapidjson::kObjectType);
		obj.AddMember("start", toJson(timepointInterval.getStartDate(), alloc), alloc);
		obj.AddMember("end", toJson(timepointInterval.getEndDate(), alloc), alloc);
		return obj;
	}
	
	template<>
	Value toJson(const Filter& filter, Document::AllocatorType& alloc)
	{
		Value obj(kObjectType);
		obj.AddMember("minTransactionNr", filter.minTransactionNr, alloc);
		obj.AddMember("maxTransactionNr", filter.maxTransactionNr, alloc);
		obj.AddMember("involvedPublicKey", toJson(*filter.involvedPublicKey, alloc), alloc);
		obj.AddMember("searchDirection", toJson(filter.searchDirection, alloc), alloc);
		obj.AddMember("pagination", toJson(filter.pagination, alloc), alloc);
		obj.AddMember("coinCommunityId", toJson(filter.coinCommunityId, alloc), alloc);
		obj.AddMember("timepointInterval", toJson(filter.timepointInterval, alloc), alloc);
		obj.AddMember("transactionType", toJson(filter.transactionType, alloc), alloc);
		obj.AddMember("filterFunction", filter.filterFunction != nullptr, alloc);

		return obj;
	}

	// Explicitly instantiate and export these specializations from the library.
	// This is required on Windows when building a dynamic/shared library to ensure
	// the symbols are visible to consuming code.
	template GRADIDOBLOCKCHAIN_EXPORT Value toJson(const Pagination& pagination, Document::AllocatorType& alloc);
	template GRADIDOBLOCKCHAIN_EXPORT Value toJson(const TimepointInterval& timepointInterval, Document::AllocatorType& alloc);
	template GRADIDOBLOCKCHAIN_EXPORT Value toJson(const Filter& filter, Document::AllocatorType& alloc);
}

