#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/data/TransferAmount.h"
#include "gradido_blockchain/data/GradidoTimeoutDeferredTransfer.h"
#include "gradido_blockchain/interaction/calculateAccountBalance/GradidoTimeoutDeferredTransferRole.h"


namespace gradido {
	namespace interaction {
		namespace calculateAccountBalance {
			GradidoUnit GradidoTimeoutDeferredTransferRole::calculateRestAmount() const
			{
				auto startAmount = getTransferAmount().getAmount();
				return startAmount.calculateDecay(mDeferredTransfer->getTimeoutDuration());
				// TODO: if a deferred transfer can be partly redeemed, add code to check how much was alreday redeemed
			}
		}
	}
}