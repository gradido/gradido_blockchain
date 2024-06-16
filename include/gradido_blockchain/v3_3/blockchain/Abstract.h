#ifndef __GRADIDO_BLOCKCHAIN_V3_3_BLOCKCHAIN_ABSTRACT_H
#define __GRADIDO_BLOCKCHAIN_V3_3_BLOCKCHAIN_ABSTRACT_H

#include "gradido_blockchain/types.h"
#include "TransactionEntry.h"
#include "FilterFunctionResult.h"
#include "SearchDirection.h"
#include "gradido_blockchain/lib/DecayDecimal.h"
#include "../data/AddressType.h"

#include <list>
#include <functional>

namespace gradido {
	namespace v3_3 {
		namespace blockchain {

			typedef std::list<std::shared_ptr<TransactionEntry>> TransactionEntries;
			class AbstractBlockchain
			{
			public:
				virtual TransactionEntries searchTransactions(
					uint64_t startTransactionNr = 0,
					std::function<FilterFunctionResult(TransactionEntry*)> filter = nullptr,
					SearchDirection order = SearchDirection::ASC
				) = 0;
				virtual data::ConfirmedTransactionPtr getLastTransaction(std::function<bool(const data::ConfirmedTransaction&)> filter = nullptr) = 0;

				//! \param maxTransactionNr don't search after this transaction id, so maxTransactionNr is last transaction included in search, 0 for not filtering
				virtual DecayDecimal calculateAddressBalance(
					memory::ConstBlockPtr accountPublicKey,
					const std::string& communityId,  // coin color
					Timepoint date,
					uint64_t maxTransactionNr = 0
				) = 0;
				//! \param maxTransactionNr don't search after this transaction id, so maxTransactionNr is last transaction included in search, 0 for not filtering
				virtual data::AddressType getAddressType(memory::ConstBlockPtr accountPublicKey, uint64_t maxTransactionNr = 0) = 0;
				virtual std::shared_ptr<TransactionEntry> getTransactionForId(uint64_t transactionId) = 0;
				//! \param coinGroupId last transaction for address filtered with coinCommunityId
				virtual std::shared_ptr<TransactionEntry> findLastTransactionForAddress(
					memory::ConstBlockPtr accountPublicKey,
					const std::string& coinCommunityId = "",
					uint64_t maxTransactionNr = 0
				) = 0;
				virtual std::shared_ptr<TransactionEntry> findByMessageId(memory::ConstBlockPtr messageId, bool cachedOnly = true) = 0;
				//! \brief Find every transaction belonging to address account in memory or block chain, expensive.
				//!
				//! Use with care, can need some time and return huge amount of data.
				//! \param address Address = user account public key.
				virtual TransactionEntries findTransactions(memory::ConstBlockPtr accountPublicKey) = 0;
				//! \brief Find transactions of account from a specific month.
				//! \param address User account public key.
				//! \param maxTransactionNr don't search after this transaction id, so maxTransactionNr is last transaction included in search, 0 for not filtering
				virtual TransactionEntries findTransactions(
					memory::ConstBlockPtr accountPublicKey,
					int month,
					int year,
					uint64_t maxTransactionNr = 0
				) = 0;
				virtual const std::string& getCommunityId() const = 0;

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

			class AccountNotFoundException : public GradidoBlockchainException
			{
			public:
				explicit AccountNotFoundException(const char* what, const std::string& groupAlias, const std::string& pubkeyHex) noexcept;
				std::string getFullString() const;
			protected:
				std::string mGroupAlias;
				std::string mPubkeyHex;
			};

			class MissingTransactionNrException : public GradidoBlockchainException
			{
			public:
				explicit MissingTransactionNrException(const char* what, uint64_t lastTransactionNr, uint64_t nextTransactionNr) noexcept;
				std::string getFullString() const;

			protected:
				uint64_t mLastTransactionNr;
				uint64_t mNextTransactionNr;
			};
		}
	}
}


#endif //__GRADIDO_BLOCKCHAIN_V3_3_BLOCKCHAIN_ABSTRACT_H