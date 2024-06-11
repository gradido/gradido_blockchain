#ifndef __GRADIDO_BLOCKCHAIN_V3_3_ABSTRACT_BLOCKCHAIN_H
#define __GRADIDO_BLOCKCHAIN_V3_3_ABSTRACT_BLOCKCHAIN_H

#include "gradido_blockchain/types.h"
#include "TransactionEntry.h"

#include <vector>
#include <functional>

namespace gradido {
	namespace v3_3 {

		class AbstractBlockchain
		{
		public:
			enum class FilterResult : uint8_t
			{
				//! put transaction in result 
				USE = 1,
				//! don't put transaction in result
				DISMISS = 2,
				//! stop search and return with result
				STOP = 4
			};			

			enum class SearchDirection : uint8_t
			{
				//! ascending order
				ASC,
				//! descending order
				DESC
			};

			virtual std::vector<std::shared_ptr<TransactionEntry>> searchTransactions(
				uint64_t startTransactionNr = 0,
				std::function<FilterResult(model::TransactionEntry*)> filter = nullptr,
				SearchDirection order = SearchDirection::ASC
			) = 0;
			virtual std::shared_ptr<gradido::ConfirmedTransaction> getLastTransaction(std::function<bool(const gradido::ConfirmedTransaction*)> filter = nullptr) = 0;
			virtual mpfr_ptr calculateAddressBalance(
				const std::string& address, 
				const std::string& groupId, 
				Timepoint date, 
				uint64_t ownTransactionNr
			) = 0;
			virtual proto::gradido::RegisterAddress_AddressType getAddressType(const std::string& address) = 0;
			virtual std::shared_ptr<TransactionEntry> getTransactionForId(uint64_t transactionId) = 0;
			virtual std::shared_ptr<TransactionEntry> findLastTransactionForAddress(const std::string& address, const std::string& groupId = "") = 0;
			virtual std::shared_ptr<TransactionEntry> findByMessageId(const MemoryBin* messageId, bool cachedOnly = true) = 0;
			//! \brief Find every transaction belonging to address account in memory or block chain, expensive.
			//!
			//! Use with care, can need some time and return huge amount of data.
			//! \param address Address = user account public key.
			virtual std::vector<std::shared_ptr<TransactionEntry>> findTransactions(const std::string& address) = 0;
			//! \brief Find transactions of account from a specific month.
			//! \param address User account public key.
			virtual std::vector<std::shared_ptr<model::TransactionEntry>> findTransactions(const std::string& address, int month, int year) = 0;
			virtual const std::string& getCommunityId() const = 0;
			[[deprecated("Replaced by getCommunityId, changed name according to Gradido Apollo implementation")]]
			inline const std::string& getGroupId() const { return getCommunityId();}

		protected:
		};

		inline AbstractBlockchain::FilterResult operator | (AbstractBlockchain::FilterResult lhs, AbstractBlockchain::FilterResult rhs)
		{
			using T = std::underlying_type_t <AbstractBlockchain::FilterResult>;
			return static_cast<AbstractBlockchain::FilterResult>(static_cast<T>(lhs) | static_cast<T>(rhs));
		}

		inline AbstractBlockchain::FilterResult operator & (AbstractBlockchain::FilterResult lhs, AbstractBlockchain::FilterResult rhs)
		{
			using T = std::underlying_type_t <AbstractBlockchain::FilterResult>;
			return static_cast<AbstractBlockchain::FilterResult>(static_cast<T>(lhs) & static_cast<T>(rhs));
		}
	}
}


#endif //__GRADIDO_BLOCKCHAIN_V3_3_ABSTRACT_BLOCKCHAIN_H