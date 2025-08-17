#include "gradido_blockchain/data/hiero/TransactionId.h"

namespace hiero {
	std::string TransactionId::toString()
	{
		std::string result;
		std::string accountIdString = mAccountId.toString();
		std::string seconds = std::to_string(mTransactionValidStart.getSeconds());
		std::string nanos = std::to_string(mTransactionValidStart.getNanos());
		result.reserve(accountIdString.size() + 2 + seconds.size() + nanos.size());
		result = accountIdString + '-' + seconds + '-' + nanos;
		return result;
	}
}