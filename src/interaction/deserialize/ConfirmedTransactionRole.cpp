#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/interaction/deserialize/AccountBalanceRole.h"
#include "gradido_blockchain/interaction/deserialize/ConfirmedTransactionRole.h"
#include "gradido_blockchain/interaction/deserialize/GradidoTransactionRole.h"
#include "gradido_blockchain/interaction/deserialize/TimestampRole.h"
#include "gradido_blockchain/interaction/deserialize/Exceptions.h"
#include "gradido_blockchain/interaction/serialize/GradidoTransactionRole.h"

using memory::Block, memory::ConstBlockPtr;
using std::make_shared;

namespace gradido {
	namespace interaction {
		namespace deserialize {

			ConfirmedTransactionRole::ConfirmedTransactionRole(const ConfirmedTransactionMessage& message)
			{
				const char* exceptionMessage = "missing member on deserialize confirmed transaction";
				ConstBlockPtr messageId = nullptr;

				if (!message["id"_f].has_value()) {
					throw MissingMemberException(exceptionMessage, "id");
				}
				if (!message["transaction"_f].has_value()) {
					throw MissingMemberException(exceptionMessage, "transaction");
				}
				if (!message["confirmed_at"_f].has_value()) {
					throw MissingMemberException(exceptionMessage, "confirmed_at");
				}
				if (!message["version_number"_f].has_value()) {
					throw MissingMemberException(exceptionMessage, "version_number");
				}
				if (!message["running_hash"_f].has_value()) {
					throw MissingMemberException(exceptionMessage, "running_hash");
				}
				if (message["message_id"_f].has_value()) {
					// throw MissingMemberException(exceptionMessage, "message_id");
					messageId = make_shared<const Block>(message["message_id"_f].value());
				}
				std::vector<data::AccountBalance> accountBalances;
				auto accountBalanceMessages = message["account_balances"_f];
				if (accountBalanceMessages.size()) {
					accountBalances.reserve(accountBalanceMessages.size());
					for (int i = 0; i < accountBalanceMessages.size(); i++) {
						accountBalances.push_back(AccountBalanceRole(accountBalanceMessages[i]));
					}
				}
				mConfirmedTransaction = std::make_shared<data::ConfirmedTransaction>(
					message["id"_f].value(),
					GradidoTransactionRole(message["transaction"_f].value()).getGradidoTransaction(),
					TimestampRole(message["confirmed_at"_f].value()).data(),
					message["version_number"_f].value(),
					make_shared<Block>(message["running_hash"_f].value()),
					messageId,
					accountBalances
				);
			}

		}
	}
}
