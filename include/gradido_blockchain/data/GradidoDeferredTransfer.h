#ifndef __GRADIDO_BLOCKCHAIN_DATA_GRADIDO_DEFERRED_TRANSFER_H
#define __GRADIDO_BLOCKCHAIN_DATA_GRADIDO_DEFERRED_TRANSFER_H

#include "GradidoTransfer.h"

namespace gradido {
	namespace data {
		class GRADIDOBLOCKCHAIN_EXPORT GradidoDeferredTransfer
		{
		public:
			GradidoDeferredTransfer(const GradidoTransfer& transfer, uint32_t timeoutDuration)
				: mTransfer(transfer), mTimeoutDuration(timeoutDuration) {}

			~GradidoDeferredTransfer() {}

			inline std::vector<memory::ConstBlockPtr> getInvolvedAddresses() const { return mTransfer.getInvolvedAddresses(); }
			inline bool isInvolved(const memory::Block& publicKey) const { return mTransfer.isInvolved(publicKey); }
			inline memory::ConstBlockPtr getSenderPublicKey() const { return mTransfer.getSender().getPublicKey(); }
			inline memory::ConstBlockPtr getRecipientPublicKey() const { return mTransfer.getRecipient(); }

			inline const GradidoTransfer& getTransfer() const { return mTransfer; }
			inline Duration getTimeoutDuration() const { return std::chrono::seconds(mTimeoutDuration); }
			inline GradidoUnit calculateUseableAmount() const { return mTransfer.getSender().getAmount().calculateDecay(getTimeoutDuration()); }

			inline bool operator==(const GradidoDeferredTransfer& other) const {
				return mTimeoutDuration == other.mTimeoutDuration && mTransfer == other.mTransfer;
			}

		protected:
			GradidoTransfer mTransfer;
			uint32_t mTimeoutDuration;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_DATA_GRADIDO_DEFERRED_TRANSFER_H