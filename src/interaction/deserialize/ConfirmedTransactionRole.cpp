#include "gradido_blockchain/interaction/deserialize/ConfirmedTransactionRole.h"
#include "gradido_blockchain/interaction/deserialize/GradidoTransactionRole.h"
#include "gradido_blockchain/interaction/deserialize/TimestampSecondsRole.h"
#include "gradido_blockchain/interaction/deserialize/Exceptions.h"

namespace gradido {
	namespace interaction {
		namespace deserialize {

			ConfirmedTransactionRole::ConfirmedTransactionRole(const ConfirmedTransactionMessage& message)
			{
				const char* exceptionMessage = "missing member on deserialize confirmed transaction";
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
				if (!message["message_id"_f].has_value()) {
					throw MissingMemberException(exceptionMessage, "message_id");
				}

				mConfirmedTransaction = std::make_shared<data::ConfirmedTransaction>(
					message["id"_f].value(),
					GradidoTransactionRole(message["transaction"_f].value()).getGradidoTransaction(),
					TimestampSecondsRole(message["confirmed_at"_f].value()).data(),
					message["version_number"_f].value(),
					std::make_shared<memory::Block>(message["running_hash"_f].value()),
					std::make_shared<memory::Block>(message["message_id"_f].value()),
					message["account_balance"_f].value_or("0")
				);
			}

		}
	}
}
