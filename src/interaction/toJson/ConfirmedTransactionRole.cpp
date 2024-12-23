#include "gradido_blockchain/data/ConfirmedTransaction.h"
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
				d.AddMember("id", mTransaction.getId(), alloc);
				if (mTransaction.getGradidoTransaction()) {
					GradidoTransactionRole gradidoTransactionToJson(*mTransaction.getGradidoTransaction().get(), mFormat);
					d.AddMember("gradidoTransaction", gradidoTransactionToJson.composeJson(rootDocument), alloc);
				}
				d.AddMember("confirmedAt", Value(DataTypeConverter::timePointToString(mTransaction.getConfirmedAt()).data(), alloc), alloc);
				d.AddMember("versionNumber", Value(mTransaction.getVersionNumber().data(), alloc), alloc);
				if (mTransaction.getRunningHash()) {
					auto hashHex = mTransaction.getRunningHash()->convertToHex();
					d.AddMember("runningHash", Value(hashHex.data(), hashHex.size(), alloc), alloc);
				}
				if (mTransaction.getMessageId()) {
					auto messageIdHex = mTransaction.getMessageId()->convertToHex();
					d.AddMember("messageId", Value(messageIdHex.data(), messageIdHex.size(), alloc), alloc);
				}
				Value accountBalances(kArrayType);
				for (auto& accountBalance : mTransaction.getAccountBalances()) {
					Value b(kObjectType);
					b.AddMember("owner", Value(enum_name(accountBalance.getOwner()).data(), alloc), alloc);
					b.AddMember("balance", Value(accountBalance.getBalance().toString().data(), alloc), alloc);
					accountBalances.PushBack(b, alloc);
				}
				d.AddMember("accountBalances", accountBalances, alloc);
					
				return d;
			}
		}
	}
}