#include "gradido_blockchain/interaction/toJson/ConfirmedTransactionRole.h"
#include "gradido_blockchain/interaction/toJson/GradidoTransactionRole.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

#include "magic_enum/magic_enum.hpp"
#include "magic_enum/magic_enum_flags.hpp"

using namespace rapidjson;
using namespace magic_enum;

namespace gradido {
	namespace interaction {
		namespace toJson {
			Value ConfirmedTransactionRole::composeJson(rapidjson::Document& rootDocument) const
			{
				Value d(kObjectType);
				auto& alloc = rootDocument.GetAllocator();
				d.AddMember("id", mTransaction.id, alloc);
				if (mTransaction.gradidoTransaction) {
					GradidoTransactionRole gradidoTransactionToJson(*mTransaction.gradidoTransaction.get(), mFormat);
					d.AddMember("gradidoTransaction", gradidoTransactionToJson.composeJson(rootDocument), alloc);
				}
				d.AddMember("confirmedAt", Value(DataTypeConverter::timePointToString(mTransaction.confirmedAt).data(), alloc), alloc);
				d.AddMember("versionNumber", Value(mTransaction.versionNumber.data(), alloc), alloc);
				if (mTransaction.runningHash) {
					d.AddMember("runningHash", Value(mTransaction.runningHash->convertToHex().data(), alloc), alloc);
				}
				if (mTransaction.messageId) {
					d.AddMember("messageId", Value(mTransaction.messageId->convertToHex().data(), alloc), alloc);
				}
				d.AddMember("accountBalance", Value(mTransaction.accountBalance.toString().data(), alloc), alloc);
					
				return d;
			}
		}
	}
}