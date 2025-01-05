#include "gradido_blockchain/const.h"
#include "gradido_blockchain/types.h"
#include "gradido_blockchain/GradidoTransactionBuilder.h"
#include "gradido_blockchain/data/GradidoTimeoutDeferredTransfer.h"
#include "gradido_blockchain/data/TransactionBody.h"
#include "gradido_blockchain/data/TransactionTriggerEvent.h"
#include "gradido_blockchain/interaction/createTransactionByEvent/TimeoutDeferredTransferEventRole.h"

using namespace std;

namespace gradido {
	using namespace data;

	namespace interaction {
		namespace createTransactionByEvent {
			std::unique_ptr<data::GradidoTransaction> TimeoutDeferredTransferEventRole::run(const blockchain::Abstract& blockchain) const
			{
				GradidoTransactionBuilder builder;
				return builder
					.setCreatedAt(mTransactionTriggerEvent->getTargetDate())
					.setTimeoutDeferredTransfer(mTransactionTriggerEvent->getLinkedTransactionId())
					.build()
					;
			}
		}
	}
}