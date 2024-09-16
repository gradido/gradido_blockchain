#ifndef __GRADIDO_BLOCKCHAIN_DATA_GRADIDO_TRANSFER_H
#define __GRADIDO_BLOCKCHAIN_DATA_GRADIDO_TRANSFER_H

#include "TransferAmount.h"

namespace gradido {
	namespace data {
		class GRADIDOBLOCKCHAIN_EXPORT GradidoTransfer
		{
		public:
			GradidoTransfer(const TransferAmount& _sender, memory::ConstBlockPtr recipientPtr);

			~GradidoTransfer() {}

			inline bool operator==(const GradidoTransfer& other) const {
				return mSender == other.mSender && mRecipient && mRecipient->isTheSame(other.mRecipient);
			}

			inline std::vector<memory::ConstBlockPtr> getInvolvedAddresses() const { return { mSender.getPubkey(), mRecipient}; }
			bool isInvolved(memory::ConstBlockPtr publicKey) const;

			inline const TransferAmount& getSender() const { return mSender; }
			inline memory::ConstBlockPtr getRecipient() const { return mRecipient; }

		protected:
			TransferAmount mSender;
			memory::ConstBlockPtr mRecipient;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_DATA_GRADIDO_TRANSFER_H