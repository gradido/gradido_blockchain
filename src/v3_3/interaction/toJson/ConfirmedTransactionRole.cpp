#include "gradido_blockchain/v3_3/interaction/toJson/ConfirmedTransactionRole.h"
#include "gradido_blockchain/v3_3/interaction/toJson/Context.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

#include "magic_enum/magic_enum.hpp"
#include "magic_enum/magic_enum_flags.hpp"

using namespace rapidjson;
using namespace magic_enum;

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace toJson {
				const char* ConfirmedTransactionRole::run(bool pretty) const
				{
					Document d(kObjectType);
					auto& alloc = d.GetAllocator();
					d.AddMember("id", mTransaction.id, alloc);
					if (mTransaction.gradidoTransaction) {
						toJson::Context gradidoTransactionToJson(*mTransaction.gradidoTransaction.get(), mFormat);
						d.AddMember("gradidoTransaction", Value(gradidoTransactionToJson.run(pretty), alloc), alloc);
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
					
					return toString(&d, pretty);
				}
			}
		}
	}
}