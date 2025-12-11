#ifndef __GRADIDO_BLOCKCHAIN_DATA_CONFIRMED_TRANSACTION_H
#define __GRADIDO_BLOCKCHAIN_DATA_CONFIRMED_TRANSACTION_H

#include "GradidoTransaction.h"
#include "AccountBalance.h"
#include "gradido_blockchain/crypto/SignatureOctet.h"

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
				Timestamp confirmedAt,
				const std::string& versionNumber,
				memory::ConstBlockPtr messageId,
				std::vector<AccountBalance> accountBalances,
				std::shared_ptr<const ConfirmedTransaction> previousConfirmedTransaction = nullptr
			);
			//! copy running hash
			ConfirmedTransaction(
				uint64_t id,
				std::shared_ptr<const GradidoTransaction> gradidoTransaction,
				Timestamp confirmedAt,
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
			inline Timestamp getConfirmedAt() const { return mConfirmedAt; } 
			inline const std::string& getVersionNumber() const { return mVersionNumber; }
			inline memory::ConstBlockPtr getRunningHash() const { return mRunningHash; }
			inline memory::ConstBlockPtr getMessageId() const { return mMessageId; }
			inline const std::vector<AccountBalance>& getAccountBalances() const { return mAccountBalances; }
			bool hasAccountBalance(const memory::Block& publicKey) const;
			//! \return accountBalance if found one with same public key or an new empty AccountBalance with this public key
			AccountBalance getAccountBalance(memory::ConstBlockPtr publicKey, const std::string& communityId) const;
			inline GradidoUnit getDecayedAccountBalance(
				memory::ConstBlockPtr publicKey,
				const std::string& communityId,
				Timepoint endDate = std::chrono::system_clock::now()
			) const;
			bool isInvolved(const memory::Block& publicKey) const;
			std::vector<memory::ConstBlockPtr> getInvolvedAddresses() const;

		protected:
			void initalizePubkeyHashes();

			uint64_t                    				mId;
			std::shared_ptr<const data::GradidoTransaction> mGradidoTransaction;
			Timestamp									mConfirmedAt;
			std::string   								mVersionNumber;
			memory::ConstBlockPtr 						mRunningHash;
			memory::ConstBlockPtr 						mMessageId;
			std::vector<AccountBalance>					mAccountBalances;

		private:
			// for faster public key comparisation
			std::vector<SignatureOctet>mPubkeyHashes;
		};

		GradidoUnit ConfirmedTransaction::getDecayedAccountBalance(
			memory::ConstBlockPtr publicKey,
			const std::string& communityId,
			Timepoint endDate/* = std::chrono::system_clock::now()*/
		) const {
			return getAccountBalance(publicKey, communityId).getBalance().calculateDecay(mConfirmedAt, endDate);
		}



		typedef std::shared_ptr<ConfirmedTransaction> ConfirmedTransactionPtr;
		typedef std::shared_ptr<const ConfirmedTransaction> ConstConfirmedTransactionPtr;
	}
}


#endif //__GRADIDO_BLOCKCHAIN_DATA_CONFIRMED_TRANSACTION_H