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
        d.AddMember("searchDirection", Value(enum_name(mFilter.searchDirection).data(), alloc), alloc);
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
        d.AddMember("transactionType", Value(enum_name(mFilter.transactionType).data(), alloc), alloc);

        return d;
      }

    }
  }
}