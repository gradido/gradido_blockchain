#include "gradido_blockchain/const.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/interaction/deserialize/AccountBalanceRole.h"
#include "gradido_blockchain/interaction/deserialize/ConfirmedTransactionRole.h"
#include "gradido_blockchain/interaction/deserialize/GradidoTransactionRole.h"
#include "gradido_blockchain/interaction/deserialize/LedgerAnchorRole.h"
#include "gradido_blockchain/interaction/deserialize/TimestampRole.h"
#include "gradido_blockchain/interaction/deserialize/Exceptions.h"
#include "gradido_blockchain/interaction/serialize/GradidoTransactionRole.h"

using memory::Block, memory::ConstBlockPtr;
using std::make_shared;

namespace gradido {
	namespace interaction {
		namespace deserialize {

			ConfirmedTransactionRole::ConfirmedTransactionRole(const ConfirmedTransactionMessage& message, uint32_t communityIdIndex)
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
				const auto& v = message["version_number"_f];
				if(!v || strcmp(v.value().data(), GRADIDO_CONFIRMED_TRANSACTION_VERSION_STRING) != 0) {
					throw InvalidMemberException("missing or invalid", "version_number", v ? v.value().data() : "", GRADIDO_CONFIRMED_TRANSACTION_VERSION_STRING);
				}
				if (!message["running_hash"_f].has_value()) {
					throw MissingMemberException(exceptionMessage, "running_hash");
				}
				std::vector<data::AccountBalance> accountBalances;
				auto accountBalanceMessages = message["account_balances"_f];
				if (accountBalanceMessages.size()) {
					accountBalances.reserve(accountBalanceMessages.size());
					for (int i = 0; i < accountBalanceMessages.size(); i++) {
						accountBalances.push_back(AccountBalanceRole(accountBalanceMessages[i], communityIdIndex));
					}
				}
				mConfirmedTransaction = std::make_shared<data::ConfirmedTransaction>(
					message["id"_f].value(),
					GradidoTransactionRole(message["transaction"_f].value(), communityIdIndex).getGradidoTransaction(),
					TimestampRole(message["confirmed_at"_f].value()).data(),
					make_shared<Block>(message["running_hash"_f].value()),
					LedgerAnchorRole(message["ledger_anchor"_f].value()),
					accountBalances,
					message["balance_derivation"_f].value()
				);
			}

		}
	}
}
