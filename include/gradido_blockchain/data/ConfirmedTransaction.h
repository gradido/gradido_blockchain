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

		protected:
			uint64_t                    				id;
			std::shared_ptr<const data::GradidoTransaction> gradidoTransaction;
			TimestampSeconds							confirmedAt;
			std::string   								versionNumber;
			memory::ConstBlockPtr 						runningHash;
			memory::ConstBlockPtr 						messageId;
			GradidoUnit   								accountBalance;
		};

		typedef std::shared_ptr<ConfirmedTransaction> ConfirmedTransactionPtr;
		typedef std::shared_ptr<const ConfirmedTransaction> ConstConfirmedTransactionPtr;
	}
}

#endif //__GRADIDO_BLOCKCHAIN_DATA_CONFIRMED_TRANSACTION_H