#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/interaction/calculateAccountBalance/GradidoRedeemDeferredTransferRole.h"

namespace gradido {
	namespace interaction {
		namespace calculateAccountBalance {
			GradidoUnit GradidoRedeemDeferredTransferRole::getAmountAdded(memory::ConstBlockPtr accountPublicKey) const
			{
				if (getRecipient()->isTheSame(accountPublicKey)) {
					return getTransferAmount().getAmount();
				}
				// return rest amount which will booked back to deferred transfer sender
				// TODO: for multiple redeeming deferred Transfer, update this logic
				if (mDeferredTransfer->getSenderPublicKey()->isTheSame(accountPublicKey)) {
					auto deferredTransferAmount = mDeferredTransfer->getTransfer().getSender().getAmount();
					auto decayedDeferredTransferAmount = deferredTransferAmount.calculateDecay(mDeferredTransferConfirmedAt, mRedeemTransferConfirmedAt);
					return decayedDeferredTransferAmount - getTransferAmount().getAmount();
				}
				return 0.0;
			}
		}
	}
}