#ifndef __GRADIDO_BLOCKCHAIN_DATA_CONFIRMED_TRANSACTION_H
#define __GRADIDO_BLOCKCHAIN_DATA_CONFIRMED_TRANSACTION_H

#include "GradidoTransaction.h"

namespace gradido {
	namespace data {

		class GRADIDOBLOCKCHAIN_EXPORT ConfirmedTransaction
		{
		public:
			ConfirmedTransaction(
				uint64_t id,
				std::shared_ptr<const GradidoTransaction> gradidoTransaction,
				Timepoint confirmedAt,
				const std::string& versionNumber,
				memory::ConstBlockPtr runningHash,
				memory::ConstBlockPtr messageId,
				const std::string& accountBalanceString
			);

			~ConfirmedTransaction() {}

			memory::Block calculateRunningHash(std::shared_ptr<const ConfirmedTransaction> previousConfirmedTransaction = nullptr) const;

			inline uint64_t getId() const { return  mId; }
			inline std::shared_ptr<const data::GradidoTransaction> getGradidoTransaction() const { return mGradidoTransaction; }
			inline TimestampSeconds getConfirmedAt() const { return mConfirmedAt; } 
			inline const std::string& getVersionNumber() const { return mVersionNumber; }
			inline memory::ConstBlockPtr getRunningHash() const { return mRunningHash; }
			inline memory::ConstBlockPtr getMessageId() const { return mMessageId; }
			inline GradidoUnit getAccountBalance() const { return mAccountBalance; }

		protected:
			uint64_t                    				mId;
			std::shared_ptr<const data::GradidoTransaction> mGradidoTransaction;
			TimestampSeconds							mConfirmedAt;
			std::string   								mVersionNumber;
			memory::ConstBlockPtr 						mRunningHash;
			memory::ConstBlockPtr 						mMessageId;
			GradidoUnit   								mAccountBalance;
		};

		typedef std::shared_ptr<ConfirmedTransaction> ConfirmedTransactionPtr;
		typedef std::shared_ptr<const ConfirmedTransaction> ConstConfirmedTransactionPtr;
	}
}

#endif //__GRADIDO_BLOCKCHAIN_DATA_CONFIRMED_TRANSACTION_H