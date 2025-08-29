#include "gradido_blockchain/data/hiero/TransactionId.h"

#include <loguru.hpp>

using namespace std;

namespace hiero {
    TransactionId::TransactionId()
        : mScheduled(false), mNonce(0)
    {

    }

    TransactionId::TransactionId(const gradido::data::Timestamp& transactionValidStart, const AccountId& accountId)
        : mTransactionValidStart(transactionValidStart), mAccountId(accountId), mScheduled(false), mNonce(0) 
    {

    }

	TransactionId::TransactionId(const std::string& transactionIdString) 
		: mAccountId(transactionIdString), mScheduled(false), mNonce(0)
	{
        const char* str = transactionIdString.c_str();
        const char* separator = strchr(str, '-'); // erstes '-' finden
        if (!separator) {
            separator = strchr(str, '@');
            if (!separator) {
                LOG_F(ERROR, "TransactionId string does not contain '-' or '@'");
                return;
            }
        }

        char* end = nullptr;

        // Number between first '-' and secondary '-', should also recognize negative seconds
        int64_t seconds = strtoll(separator + 1, &end, 10);
        if (!end || ( *end != '-' && *end != '.')) {
            LOG_F(ERROR, "Expected '-' or '.' after seconds, got: %c", end ? *end : '\0');
            return;
        }

        // Number after second '-' or '.'
        int32_t nanos = static_cast<int32_t>(strtol(end + 1, &end, 10));
        if (!end) {
            LOG_F(WARNING, "Parsing nonce may have failed");
        }
        mTransactionValidStart = gradido::data::Timestamp(seconds, nanos);
	}

    TransactionId::~TransactionId() 
    {

    }

	std::string TransactionId::toString() const
	{
		std::string result;
		std::string accountIdString = mAccountId.toString();
		std::string seconds = std::to_string(mTransactionValidStart.getSeconds());
		std::string nanos = std::to_string(mTransactionValidStart.getNanos());
		result.reserve(accountIdString.size() + 2 + seconds.size() + nanos.size());
		result = accountIdString + '@' + seconds + '.' + nanos;
		return result;
	}
}