#include "gradido_blockchain/v3_3/AbstractBlockchain.h"

namespace gradido {
	namespace v3_3 {
	
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

	}
}