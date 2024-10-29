#ifndef __GRADIDO_BLOCKCHAIN_DATA_GRADIDO_DEFERRED_TRANSFER_H
#define __GRADIDO_BLOCKCHAIN_DATA_GRADIDO_DEFERRED_TRANSFER_H

#include "GradidoTransfer.h"
#include "TimestampSeconds.h"

namespace gradido {
	namespace data {
		class GRADIDOBLOCKCHAIN_EXPORT GradidoDeferredTransfer
		{
		public:
			GradidoDeferredTransfer(const GradidoTransfer& transfer, Timepoint timeout)
				: mTransfer(transfer), mTimeout(timeout) {}

			~GradidoDeferredTransfer() {}

			inline std::vector<memory::ConstBlockPtr> getInvolvedAddresses() const { return mTransfer.getInvolvedAddresses(); }
			inline bool isInvolved(const memory::Block& publicKey) const { return mTransfer.isInvolved(publicKey); }
			inline memory::ConstBlockPtr getSenderPublicKey() const { return mTransfer.getSender().getPubkey(); }
			inline memory::ConstBlockPtr getRecipientPublicKey() const { return mTransfer.getRecipient(); }

			inline const GradidoTransfer& getTransfer() const { return mTransfer; }
			inline TimestampSeconds getTimeout() const { return mTimeout; }

		protected:
			GradidoTransfer mTransfer;
			TimestampSeconds mTimeout;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_DATA_GRADIDO_DEFERRED_TRANSFER_H