#ifndef __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_EXCEPTIONS_H
#define __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_EXCEPTIONS_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

namespace gradido {
	namespace blockchain {
		class Filter;

		class GRADIDOBLOCKCHAIN_EXPORT ConstructBlockchainException : public GradidoBlockchainException
		{
		public:
			explicit ConstructBlockchainException(const char* what, std::string_view communityId) noexcept
				: GradidoBlockchainException(what), mCommunityId(communityId) {}

			std::string getFullString() const {
				std::string result = what();
				result += ", communityId: " + mCommunityId;
				return result;
			}
		protected:
			std::string mCommunityId;
		};

		class GRADIDOBLOCKCHAIN_EXPORT AccountNotFoundException : public GradidoBlockchainException
		{
		public:
			explicit AccountNotFoundException(const char* what, const std::string& groupAlias, const std::string& pubkeyHex) noexcept;
			std::string getFullString() const;
		protected:
			std::string mGroupAlias;
			std::string mPubkeyHex;
		};

		class GRADIDOBLOCKCHAIN_EXPORT MissingTransactionNrException : public GradidoBlockchainException
		{
		public:
			explicit MissingTransactionNrException(const char* what, uint64_t lastTransactionNr, uint64_t nextTransactionNr) noexcept;
			std::string getFullString() const;

		protected:
			uint64_t mLastTransactionNr;
			uint64_t mNextTransactionNr;
		};

		class GRADIDOBLOCKCHAIN_EXPORT TransactionResultCountException : public GradidoBlockchainException
		{
		public:
			explicit TransactionResultCountException(const char* what, int expectedResultCount, int actuallyResultCount, const Filter& filter) noexcept;

			std::string getFullString() const;
		protected:
			int mExpectedResultCount;
			int mActuallyResultCount;
			std::string mFilterJson;
		};

		class GRADIDOBLOCKCHAIN_EXPORT CommunityNotFoundException : GradidoBlockchainException
		{
		public:
			explicit CommunityNotFoundException(const char* what, std::string_view communityId) noexcept;
			explicit CommunityNotFoundException(const char* what, uint32_t communityIdIndex) noexcept;
			std::string getFullString() const;

		protected:
			std::string mCommunityId;

		};
	}
}
#endif //__GRADIDO_BLOCKCHAIN_BLOCKCHAIN_EXCEPTIONS_H