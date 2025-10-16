#include "gradido_blockchain/blockchain/Filter.h"
#include "gradido_blockchain/blockchain/Exceptions.h"
#include "gradido_blockchain/serialization/toJsonString.h"

namespace gradido {
	namespace blockchain {

		AccountNotFoundException::AccountNotFoundException(const char* what, const std::string& groupAlias, const std::string& pubkeyHex) noexcept
			: GradidoBlockchainException(what), mGroupAlias(groupAlias), mPubkeyHex(pubkeyHex)
		{

		}

		std::string AccountNotFoundException::getFullString() const
		{
			std::string result = what();
			result += ", group alias: " + mGroupAlias;
			result += ", pubkey: " + mPubkeyHex;
			return std::move(result);
		}

		MissingTransactionNrException::MissingTransactionNrException(const char* what, uint64_t lastTransactionNr, uint64_t nextTransactionNr) noexcept
			: GradidoBlockchainException(what), mLastTransactionNr(lastTransactionNr), mNextTransactionNr(nextTransactionNr)
		{

		}

		std::string MissingTransactionNrException::getFullString() const
		{
			std::string result = what();
			result += ", hole between transaction nr: " + std::to_string(mLastTransactionNr);
			result += " and transaction nr: " + std::to_string(mNextTransactionNr);
			return result;
		}

		TransactionResultCountException::TransactionResultCountException(
			const char* what, int expectedResultCount, int actuallyResultCount, const Filter& filter
		) noexcept
			: GradidoBlockchainException(what), 
			mExpectedResultCount(expectedResultCount), 
			mActuallyResultCount(actuallyResultCount), 
			mFilterJson(serialization::toJsonString(filter, true))
		{
		}

		std::string TransactionResultCountException::getFullString() const
		{
			std::string result;
			result.reserve(mFilterJson.size() + strlen(what()) + 60);
			result = what();
			result += ", expected result count: " + std::to_string(mExpectedResultCount);
			result += " actual result count: " + std::to_string(mActuallyResultCount);
			result += "\n" + mFilterJson;
			return result;
		}

		CommunityNotFoundException::CommunityNotFoundException(const char* what, std::string_view communityId) noexcept
			: GradidoBlockchainException(what), mCommunityId(communityId)
		{

		}

		std::string CommunityNotFoundException::getFullString() const
		{
			std::string result = what();
			result += ", community: " + mCommunityId;
			return result;
		}

	}
}