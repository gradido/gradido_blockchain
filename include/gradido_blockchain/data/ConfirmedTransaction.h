#ifndef __GRADIDO_BLOCKCHAIN_DATA_CONFIRMED_TRANSACTION_H
#define __GRADIDO_BLOCKCHAIN_DATA_CONFIRMED_TRANSACTION_H

#include "GradidoTransaction.h"
#include "AccountBalance.h"

namespace gradido {
	namespace data {

		class GRADIDOBLOCKCHAIN_EXPORT ConfirmedTransaction
		{
		public:
			//! calculate running hash
			//! \param previousConfirmedTransaction needed for running hash calculation
			ConfirmedTransaction(
				uint64_t id,
				std::shared_ptr<const GradidoTransaction> gradidoTransaction,
				Timepoint confirmedAt,
				const std::string& versionNumber,
				memory::ConstBlockPtr messageId,
				std::vector<AccountBalance> accountBalances,
				std::shared_ptr<const ConfirmedTransaction> previousConfirmedTransaction = nullptr
			);
			//! copy running hash
			ConfirmedTransaction(
				uint64_t id,
				std::shared_ptr<const GradidoTransaction> gradidoTransaction,
				Timepoint confirmedAt,
				const std::string& versionNumber,
				memory::ConstBlockPtr runningHash,
				memory::ConstBlockPtr messageId,
				std::vector<AccountBalance> accountBalances
			);

			~ConfirmedTransaction() {}

			memory::ConstBlockPtr calculateRunningHash(
				std::shared_ptr<const ConfirmedTransaction> previousConfirmedTransaction = nullptr
			) const;

			inline uint64_t getId() const { return  mId; }
			inline std::shared_ptr<const data::GradidoTransaction> getGradidoTransaction() const { return mGradidoTransaction; }
			inline TimestampSeconds getConfirmedAt() const { return mConfirmedAt; } 
			inline const std::string& getVersionNumber() const { return mVersionNumber; }
			inline memory::ConstBlockPtr getRunningHash() const { return mRunningHash; }
			inline memory::ConstBlockPtr getMessageId() const { return mMessageId; }
			inline const std::vector<AccountBalance>& getAccountBalances() const { return mAccountBalances; }

		protected:
			uint64_t                    				mId;
			std::shared_ptr<const data::GradidoTransaction> mGradidoTransaction;
			TimestampSeconds							mConfirmedAt;
			std::string   								mVersionNumber;
			memory::ConstBlockPtr 						mRunningHash;
			memory::ConstBlockPtr 						mMessageId;
			std::vector<AccountBalance>					mAccountBalances;
		};

		typedef std::shared_ptr<ConfirmedTransaction> ConfirmedTransactionPtr;
		typedef std::shared_ptr<const ConfirmedTransaction> ConstConfirmedTransactionPtr;
	}
}

#endif //__GRADIDO_BLOCKCHAIN_DATA_CONFIRMED_TRANSACTION_H