#ifndef __GRADIDO_BLOCKCHAIN_DATA_GRADIDO_CREATION_H
#define __GRADIDO_BLOCKCHAIN_DATA_GRADIDO_CREATION_H

#include "TransferAmount.h"
#include "TimestampSeconds.h"

namespace gradido {
	namespace data {
		class GRADIDOBLOCKCHAIN_EXPORT GradidoCreation
		{
		public:
			GradidoCreation(const TransferAmount& recipient, Timepoint targetDate)
				: mRecipient(recipient), mTargetDate(targetDate) {}

			~GradidoCreation() {}

			inline std::vector<memory::ConstBlockPtr> getInvolvedAddresses() const { return { mRecipient.getPubkey()}; }
			inline bool isInvolved(const memory::Block& publicKey) const { return publicKey.isTheSame(mRecipient.getPubkey()); }

			inline const TransferAmount& getRecipient() const { return mRecipient; }
			inline TimestampSeconds getTargetDate() const { return mTargetDate; }

		protected:
			TransferAmount mRecipient;
			TimestampSeconds mTargetDate;
		};
	}
}

#endif // __GRADIDO_BLOCKCHAIN_DATA_GRADIDO_CREATION_H