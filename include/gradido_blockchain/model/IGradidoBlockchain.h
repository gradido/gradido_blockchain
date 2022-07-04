#ifndef __GRADIDO_BLOCKCHAIN_MODEL_I_GRADIDO_BLOCKCHAIN_H
#define __GRADIDO_BLOCKCHAIN_MODEL_I_GRADIDO_BLOCKCHAIN_H

//#include "protobufWrapper/GradidoBlock.h"
#include "protobufWrapper/TransactionBase.h"
#include "TransactionEntry.h"
#include "Poco/SharedPtr.h"
#include <vector>

namespace model {

	class IGradidoBlockchain
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

		virtual std::vector<Poco::SharedPtr<TransactionEntry>> searchTransactions(
			uint64_t startTransactionNr = 0,
			std::function<FilterResult(model::TransactionEntry*)> filter = nullptr,
			SearchDirection order = SearchDirection::ASC
		) = 0;
		virtual Poco::SharedPtr<gradido::GradidoBlock> getLastTransaction(std::function<bool(const gradido::GradidoBlock*)> filter = nullptr) = 0;
		virtual mpfr_ptr calculateAddressBalance(const std::string& address, const std::string& groupId, Poco::DateTime date) = 0;
		virtual proto::gradido::RegisterAddress_AddressType getAddressType(const std::string& address) = 0;
		virtual Poco::SharedPtr<TransactionEntry> getTransactionForId(uint64_t transactionId) = 0;
		virtual Poco::SharedPtr<TransactionEntry> findLastTransactionForAddress(const std::string& address, const std::string& groupId = "") = 0;
		virtual Poco::SharedPtr<TransactionEntry> findByMessageId(const MemoryBin* messageId, bool cachedOnly = true) = 0;
		//! \brief Find every transaction belonging to address account in memory or block chain, expensive.
		//!
		//! Use with care, can need some time and return huge amount of data.
		//! \param address Address = user account public key.
		virtual std::vector<Poco::SharedPtr<TransactionEntry>> findTransactions(const std::string& address) = 0;
		//! \brief Find transactions of account from a specific month.
		//! \param address User account public key.
		virtual std::vector<Poco::SharedPtr<model::TransactionEntry>> findTransactions(const std::string& address, int month, int year) = 0;
		virtual const std::string& getGroupId() const = 0;

	protected:
	};

	inline IGradidoBlockchain::FilterResult operator | (IGradidoBlockchain::FilterResult lhs, IGradidoBlockchain::FilterResult rhs)
	{
		using T = std::underlying_type_t <IGradidoBlockchain::FilterResult>;
		return static_cast<IGradidoBlockchain::FilterResult>(static_cast<T>(lhs) | static_cast<T>(rhs));
	}

	inline IGradidoBlockchain::FilterResult operator & (IGradidoBlockchain::FilterResult lhs, IGradidoBlockchain::FilterResult rhs)
	{
		using T = std::underlying_type_t <IGradidoBlockchain::FilterResult>;
		return static_cast<IGradidoBlockchain::FilterResult>(static_cast<T>(lhs) & static_cast<T>(rhs));
	}
}

#endif //__GRADIDO_BLOCKCHAIN_MODEL_I_GRADIDO_BLOCKCHAIN_H