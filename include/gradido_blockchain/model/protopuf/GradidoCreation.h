#ifndef __GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_GRADIDO_CREATION_H
#define __GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_GRADIDO_CREATION_H

#include "TransferAmount.h"
#include "TimestampSeconds.h"

namespace model {
	namespace protopuf {

		using GradidoCreationMessage = message<
			message_field<"recipient", 1, TransferAmountMessage>,
			message_field<"target_date", 3, TimestampSecondsMessage>
		>;

		class GRADIDOBLOCKCHAIN_EXPORT GradidoCreation
		{
		public:
			GradidoCreation(const GradidoCreationMessage& data);
			GradidoCreation(const TransferAmount& recipient, Timepoint targetDate);

			~GradidoCreation() {}

			inline Timepoint getTargetDate() const { return mTargetDate.getAsTimepoint(); }
			inline const TransferAmount getRecipient() const { return mRecipient; }

		protected:
			TransferAmount mRecipient;
			TimestampSeconds mTargetDate;
		};
	}
}



#endif //__GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_GRADIDO_CREATION_H