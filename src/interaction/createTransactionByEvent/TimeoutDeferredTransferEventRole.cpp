#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/const.h"
#include "gradido_blockchain/types.h"
#include "gradido_blockchain/GradidoTransactionBuilder.h"
#include "gradido_blockchain/data/GradidoTimeoutDeferredTransfer.h"
#include "gradido_blockchain/data/GradidoTransaction.h"
#include "gradido_blockchain/data/TransactionBody.h"
#include "gradido_blockchain/data/TransactionTriggerEvent.h"
#include "gradido_blockchain/interaction/createTransactionByEvent/TimeoutDeferredTransferEventRole.h"

using std::unique_ptr;

namespace gradido {
	using namespace data;

	namespace interaction {
		namespace createTransactionByEvent {
			unique_ptr<data::GradidoTransaction> TimeoutDeferredTransferEventRole::run(const blockchain::Abstract& blockchain) const
			{
				GradidoTransactionBuilder builder;
				return builder
					.setCreatedAt(mTransactionTriggerEvent->getTargetDate())
					.setSenderCommunity(blockchain.getCommunityIdIndex())
					.setTimeoutDeferredTransfer(mTransactionTriggerEvent->getLinkedTransactionId())
					.build()
					;
			}
		}
	}
}