#ifndef __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_TRANSACTION_ENTRY_H
#define __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_TRANSACTION_ENTRY_H

#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/data/TransactionType.h"
#include "../data/EventTriggeredTransaction.h"

#include "date/date.h"

#include <vector>
#include <list>

namespace gradido {
	namespace blockchain {

		/*!
		* @author Dario Rekowski
		* @date 2020-03-12
		* @brief container for transaction + index details
		*
		* Used between multiple controller while write to block is pending and while it is cached
		* It contains the serialized transaction and multiple data for fast indexing
		*/
		class GRADIDOBLOCKCHAIN_EXPORT TransactionEntry
		{
		public:
			TransactionEntry()
				: mTransactionNr(0), mMonth(0), mYear(0), mTransactionType(data::TransactionType::NONE) {}

			//! \brief init entry object from serialized transaction, deserialize transaction to get infos
			TransactionEntry(memory::ConstBlockPtr serializedTransaction);

			TransactionEntry(data::ConstConfirmedTransactionPtr confirmedTransaction);

			TransactionEntry(std::shared_ptr<data::EventTriggeredTransaction> eventTriggeredTransaction);

			//! \brief init entry object without indices
			TransactionEntry(
				uint64_t transactionNr,
				date::month month,
				date::year year,
				data::TransactionType transactionType,
				std::string communityId
			);
			virtual ~TransactionEntry() {}

			//! \brief operator for sorting by mTransactionNr in ascending order
			bool operator < (const TransactionEntry& b) const { return mTransactionNr < b.mTransactionNr; }
			bool operator > (const TransactionEntry& b) const { return mTransactionNr > b.mTransactionNr; }

			inline uint64_t getTransactionNr() const { return mTransactionNr; }
			inline memory::ConstBlockPtr getSerializedTransaction() const { return mSerializedTransaction; }
			data::ConstConfirmedTransactionPtr getConfirmedTransaction() const;
			inline date::month getMonth() const { return mMonth; }
			inline date::year getYear() const { return mYear; }
			inline data::TransactionType getTransactionType() const { return mTransactionType; }
			inline std::string_view getCoinCommunityId() const { return mCoinCommunityId; }
			static std::string getCoinCommunityId(const data::TransactionBody& body);
			inline data::ConstTransactionBodyPtr getTransactionBody() const { return getConfirmedTransaction()->getGradidoTransaction()->getTransactionBody(); }

			// default transactions
			inline bool isTransfer() const { return mTransactionType == data::TransactionType::TRANSFER; }
			inline bool isCreation() const { return mTransactionType == data::TransactionType::CREATION; }
			inline bool isCommunityFriendsUpdate() const { return mTransactionType == data::TransactionType::COMMUNITY_FRIENDS_UPDATE; }
			inline bool isRegisterAddress() const { return mTransactionType == data::TransactionType::REGISTER_ADDRESS; }
			inline bool isDeferredTransfer() const { return mTransactionType == data::TransactionType::DEFERRED_TRANSFER; }
			inline bool isCommunityRoot() const { return mTransactionType == data::TransactionType::COMMUNITY_ROOT; }
			inline bool isRedeemDeferredTransfer() const { return mTransactionType == data::TransactionType::REDEEM_DEFERRED_TRANSFER; }

			// event triggered transactions
			inline bool isDeferredTimeoutReversal() const;
			inline bool isDeferredRedeemReversal() const;

		protected:
			uint64_t mTransactionNr;
			memory::ConstBlockPtr mSerializedTransaction;
			date::month mMonth;
			date::year mYear;
			data::TransactionType mTransactionType;
			std::string mCoinCommunityId;
			mutable std::mutex mFastMutex;
			mutable data::ConstConfirmedTransactionPtr mConfirmedTransaction;
			mutable std::shared_ptr<data::EventTriggeredTransaction> mEventTriggeredTransaction;
		};

		bool TransactionEntry::isDeferredTimeoutReversal() const {
			return mEventTriggeredTransaction && 
				mEventTriggeredTransaction->getType() == data::EventTriggeredTransactionType::DEFERRED_TIMEOUT_REVERSAL;
		}

		bool TransactionEntry::isDeferredRedeemReversal() const {
			return mEventTriggeredTransaction &&
				mEventTriggeredTransaction->getType() == data::EventTriggeredTransactionType::DEFERRED_REDEEM_REVERSAL;
		}


		typedef std::list<std::shared_ptr<const TransactionEntry>> TransactionEntries;
		typedef std::pair<std::shared_ptr<const TransactionEntry>, std::shared_ptr<const TransactionEntry>> DeferredRedeemedTransferPair;
	}
}

#endif //__GRADIDO_BLOCKCHAIN_BLOCKCHAIN_TRANSACTION_ENTRY_H
