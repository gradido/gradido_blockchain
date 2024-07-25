#ifndef __GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_GRADIDO_DEFERRED_TRANSFER_H
#define __GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_GRADIDO_DEFERRED_TRANSFER_H

#include "GradidoTransfer.h"
#include "TimestampSeconds.h"

namespace model {
	namespace protopuf {

		using GradidoDeferredTransferMessage = message<
			message_field<"transfer", 1, GradidoTransferMessage>,
			message_field<"timeout", 2, TimestampSecondsMessage>
		>;

		class GRADIDOBLOCKCHAIN_EXPORT GradidoDeferredTransfer
		{
		public:
			GradidoDeferredTransfer(const GradidoDeferredTransferMessage& data);
			GradidoDeferredTransfer(const GradidoTransfer& transfer, Timepoint date);

			~GradidoDeferredTransfer() {}

			inline const GradidoTransfer& getTransfer() { return mParent; }
			inline Timepoint getTimeout() { return mTimeout.getAsTimepoint(); }

		protected:
			GradidoTransfer mParent;
			TimestampSeconds mTimeout;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_GRADIDO_DEFERRED_TRANSFER_H