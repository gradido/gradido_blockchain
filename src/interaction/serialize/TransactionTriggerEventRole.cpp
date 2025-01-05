#include "gradido_blockchain/interaction/serialize/TransactionTriggerEventRole.h"
#include "gradido_blockchain/data/Timestamp.h"
#include "gradido_blockchain/data/TransactionTriggerEvent.h"

namespace gradido {
	namespace interaction {
		namespace serialize {
			TransactionTriggerEventRole::TransactionTriggerEventRole(const data::TransactionTriggerEvent& transactionTriggerEvent)
				: mTransactionTriggerEvent(transactionTriggerEvent)
			{

			}

			TransactionTriggerEventRole::~TransactionTriggerEventRole()
			{

			}

			TransactionTriggerEventMessage TransactionTriggerEventRole::getMessage() const
			{
				auto targetDate = data::Timestamp(mTransactionTriggerEvent.getTargetDate());
				return TransactionTriggerEventMessage(
					mTransactionTriggerEvent.getLinkedTransactionId(),
					TimestampMessage(targetDate.getSeconds(), targetDate.getNanos()),
					mTransactionTriggerEvent.getType()
				);
			}

			size_t TransactionTriggerEventRole::calculateSerializedSize() const
			{
				return sizeof(uint64_t) + sizeof(data::Timestamp) + sizeof(data::TransactionTriggerEventType) + 8;
			}
		}
	}
}