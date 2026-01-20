#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/blockchain/Filter.h"
#include "gradido_blockchain/blockchain/Exceptions.h"
#include "gradido_blockchain/serialization/toJsonString.h"

#include <string>
#include <string_view>

using std::string, std::string_view, std::to_string;

namespace gradido {
	namespace blockchain {

		AccountNotFoundException::AccountNotFoundException(const char* what, const string& groupAlias, const string& pubkeyHex) noexcept
			: GradidoBlockchainException(what), mGroupAlias(groupAlias), mPubkeyHex(pubkeyHex)
		{

		}

		string AccountNotFoundException::getFullString() const
		{
			string result = what();
			result += ", group alias: " + mGroupAlias;
			result += ", pubkey: " + mPubkeyHex;
			return std::move(result);
		}

		MissingTransactionNrException::MissingTransactionNrException(const char* what, uint64_t lastTransactionNr, uint64_t nextTransactionNr) noexcept
			: GradidoBlockchainException(what), mLastTransactionNr(lastTransactionNr), mNextTransactionNr(nextTransactionNr)
		{

		}

		string MissingTransactionNrException::getFullString() const
		{
			string result = what();
			result += ", hole between transaction nr: " + to_string(mLastTransactionNr);
			result += " and transaction nr: " + to_string(mNextTransactionNr);
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

		string TransactionResultCountException::getFullString() const
		{
			string result;
			result.reserve(mFilterJson.size() + strlen(what()) + 60);
			result = what();
			result += ", expected result count: " + std::to_string(mExpectedResultCount);
			result += " actual result count: " + std::to_string(mActuallyResultCount);
			result += "\n" + mFilterJson;
			return result;
		}

		CommunityNotFoundException::CommunityNotFoundException(const char* what, string_view communityId) noexcept
			: GradidoBlockchainException(what), mCommunityId(communityId)
		{

		}

		CommunityNotFoundException::CommunityNotFoundException(const char* what, uint32_t communityIdIndex) noexcept
			: GradidoBlockchainException(what)
		{
			auto communityId = g_appContext->getCommunityIds().getDataForIndex(communityIdIndex);
			if (communityId.has_value()) {
				mCommunityId = communityId.value();
			}
			else {
				mCommunityId = to_string(communityIdIndex);
			}
		}

		string CommunityNotFoundException::getFullString() const
		{
			string result = what();
			result += ", community: " + mCommunityId;
			return result;
		}

	}
}