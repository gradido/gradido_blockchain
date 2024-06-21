#include "gradido_blockchain/v3_3/interaction/calculateAccountBalance/GradidoDeferredTransferRole.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace calculateAccountBalance {

				DecayDecimal GradidoDeferredTransferRole::getBalance() const
				{
					// if timeout is reached, balance is zero
					// if deferred transfer wasn't redeemed, it was booked back to sender,
					// if it was redeemed, it is booked away to redeem account
					auto timeout = mDeferredTransfer.timeout.getAsTimepoint();
					if (timeout <= mBalanceDate) {
						return static_cast<long>(0);
					}
					
					// subtract decay from time: balanceDate -> timeout
					// later in code the decay for: received -> balanceDate will be subtracted automatic
					// deferred transfers contain redeemable amount + decay for createdAt -> timeout as fee
					return mDeferredTransfer.transfer.sender.amount.calculateDecay(mBalanceDate, timeout);
				}
			}
		}
	}
}
	 