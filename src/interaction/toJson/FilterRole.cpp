#include "gradido_blockchain/blockchain/Filter.h"
#include "gradido_blockchain/interaction/toJson/FilterRole.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

#include "magic_enum/magic_enum.hpp"

using namespace rapidjson;
using namespace magic_enum;

namespace gradido {
	namespace interaction {
		namespace toJson {
			Value FilterRole::composeJson(rapidjson::Document& rootDocument) const
      {
        Value d(kObjectType);
				auto& alloc = rootDocument.GetAllocator();
        d.AddMember("minTransactionNr", mFilter.minTransactionNr, alloc);
        d.AddMember("maxTransactionNr", mFilter.maxTransactionNr, alloc);
        if(mFilter.involvedPublicKey) {
          d.AddMember("involvedPublicKey", Value(mFilter.involvedPublicKey->convertToHex().data(), alloc), alloc);
        }
        auto searchDirectionName = enum_name(mFilter.searchDirection);
        d.AddMember("searchDirection", Value(searchDirectionName.data(), searchDirectionName.size(), alloc), alloc);
        Value pagination(kObjectType);
        pagination.AddMember("size", mFilter.pagination.size, alloc);
        pagination.AddMember("page", mFilter.pagination.page, alloc);
        d.AddMember("pagination", pagination, alloc);
        d.AddMember("coinCommunityId", Value(mFilter.coinCommunityId.data(), alloc), alloc);

        Value timepointInterval(kObjectType);
        timepointInterval.AddMember(
          "startDate",
          Value(DataTypeConverter::timePointToString(mFilter.timepointInterval.getStartDate()).data(), alloc),
          alloc
        );
        timepointInterval.AddMember(
          "endDate",
          Value(DataTypeConverter::timePointToString(mFilter.timepointInterval.getEndDate()).data(), alloc),
          alloc
        );
        d.AddMember("timepointInterval", timepointInterval, alloc);
        auto transactionType = enum_name(mFilter.transactionType);
        d.AddMember("transactionType", Value(transactionType.data(), transactionType.size(), alloc), alloc);

        return d;
      }

    }
  }
}