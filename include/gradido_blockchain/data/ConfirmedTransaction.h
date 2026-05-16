#ifndef __GRADIDO_BLOCKCHAIN_DATA_CONFIRMED_TRANSACTION_H
#define __GRADIDO_BLOCKCHAIN_DATA_CONFIRMED_TRANSACTION_H

#include "GradidoTransaction.h"
#include "AccountBalance.h"
#include "BalanceDerivationType.h"
#include "LedgerAnchor.h"
#include "gradido_blockchain/crypto/SignatureOctet.h"

#include <optional>

struct grdw_confirmed_transaction;
struct grd_memory;

namespace gradido {
	namespace data {

		namespace compact {
			struct PublicKeyIndex;
		}

		class GRADIDOBLOCKCHAIN_EXPORT ConfirmedTransaction
		{
		public:
			//! calculate running hash
			//! \param previousConfirmedTransaction needed for running hash calculation
			ConfirmedTransaction(
				uint64_t id,
				std::shared_ptr<const GradidoTransaction> gradidoTransaction,
				Timestamp confirmedAt,
				const LedgerAnchor& ledgerAnchor,
				std::vector<AccountBalance> accountBalances,
				BalanceDerivationType balanceDerivationType,
				std::shared_ptr<const ConfirmedTransaction> previousConfirmedTransaction = nullptr
			);
			//! copy running hash
			ConfirmedTransaction(
				uint64_t id,
				std::shared_ptr<const GradidoTransaction> gradidoTransaction,
				Timestamp confirmedAt,
				memory::ConstBlockPtr runningHash,
				const LedgerAnchor& ledgerAnchor,
				std::vector<AccountBalance> accountBalances,
				BalanceDerivationType balanceDerivationType
			);

			~ConfirmedTransaction() {}
			static std::shared_ptr<const ConfirmedTransaction> fromGrdw(const grdw_confirmed_transaction* grdw_tx, uint32_t communityIdIndex);
			void toGrdw(grd_memory* alloc, grdw_confirmed_transaction* grdw_tx, uint32_t communityIdIndex) const;

			memory::ConstBlockPtr calculateRunningHash(
				std::shared_ptr<const ConfirmedTransaction> previousConfirmedTransaction = nullptr
			) const;

			inline uint64_t getId() const { return  mId; }
			inline std::shared_ptr<const data::GradidoTransaction> getGradidoTransaction() const { return mGradidoTransaction; }
			inline Timestamp getConfirmedAt() const { return mConfirmedAt; } 
			inline memory::ConstBlockPtr getRunningHash() const { return mRunningHash; }
			inline const LedgerAnchor& getLedgerAnchor() const { return mLedgerAnchor; }
			inline const std::vector<AccountBalance>& getAccountBalances() const { return mAccountBalances; }
			bool hasAccountBalance(const memory::Block& publicKey, std::optional<uint32_t> communityIdIndex) const;
			//! \return accountBalance if found one with same public key or an new empty AccountBalance with this public key
			AccountBalance getAccountBalance(memory::ConstBlockPtr publicKey, std::optional<uint32_t> communityIdIndex) const;
			AccountBalance getAccountBalance(memory::ConstBlockPtr publicKey, const std::string& communityIdIndex) const;
			inline GradidoUnit getDecayedAccountBalance(
				memory::ConstBlockPtr publicKey,
				std::optional<uint32_t> coinCommunityIdIndex,
				Timepoint endDate = std::chrono::system_clock::now()
			) const;
			BalanceDerivationType getBalanceDerivationType() const { return mBalanceDerivationType; }
			bool isBalanceNodeComputed() const { return BalanceDerivationType::NODE == mBalanceDerivationType; }
			bool isBalanceExternComputed() const { return BalanceDerivationType::EXTERN == mBalanceDerivationType; }
			[[deprecated("Replaced by isInvolved with compact::PublicKeyIndex")]]
			bool isInvolved(const memory::Block& publicKey) const;
			bool isInvolved(const compact::PublicKeyIndex publicKeyIndex) const;
			bool isBalanceUpdated(const memory::Block& publicKey) const;
			bool isBalanceUpdated(const compact::PublicKeyIndex publicKeyIndex) const;
			[[deprecated("Replaced by getInvolvedAddressIndices")]]
			std::vector<memory::ConstBlockPtr> getInvolvedAddresses() const;
			std::vector<compact::PublicKeyIndex> getInvolvedAddressIndices() const;
			bool isTheSame(const ConfirmedTransaction& other) const;

		protected:
			// empty constructor
			void initalizePubkeyHashes();

			uint64_t                    				mId;
			std::shared_ptr<const data::GradidoTransaction> mGradidoTransaction;
			Timestamp									mConfirmedAt;
			memory::ConstBlockPtr 						mRunningHash;
			LedgerAnchor								mLedgerAnchor;
			std::vector<AccountBalance>					mAccountBalances;
			BalanceDerivationType						mBalanceDerivationType;

		private:
			// for faster public key comparisation
			std::vector<compact::PublicKeyIndex> mPubkeyIndices;
		};

		GradidoUnit ConfirmedTransaction::getDecayedAccountBalance(
			memory::ConstBlockPtr publicKey,
			std::optional<uint32_t> coinCommunityIdIndex,
			Timepoint endDate/* = std::chrono::system_clock::now()*/
		) const {
			return getAccountBalance(publicKey, coinCommunityIdIndex).getBalance().calculateDecay(mConfirmedAt, endDate);
		}
		typedef std::shared_ptr<ConfirmedTransaction> ConfirmedTransactionPtr;
		typedef std::shared_ptr<const ConfirmedTransaction> ConstConfirmedTransactionPtr;
	}
}


#endif //__GRADIDO_BLOCKCHAIN_DATA_CONFIRMED_TRANSACTION_H