#include "gradido_blockchain/v3_3/interaction/deserialize/ConfirmedTransactionRole.h"
#include "gradido_blockchain/v3_3/interaction/deserialize/GradidoTransactionRole.h"
#include "gradido_blockchain/v3_3/interaction/deserialize/TimestampSecondsRole.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace deserialize {

				ConfirmedTransactionRole::ConfirmedTransactionRole(const ConfirmedTransactionMessage& message)
				{
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
}
