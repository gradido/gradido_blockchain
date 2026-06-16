#include "gradido_blockchain/data/hiero/TransactionId.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain_core/data/wire/hiero.h"

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

    TransactionId::TransactionId(const grdw_hiero_transaction_id& coreHieroGradidoId)
      : mTransactionValidStart(coreHieroGradidoId.transactionValidStart), mAccountId(coreHieroGradidoId.accountID)
    {

    }

    TransactionId::~TransactionId()
    {

    }

	  std::string TransactionId::toString() const
	  {
			grdw_hiero_transaction_id hieroTxId = {
			  .transactionValidStart = {
					.seconds = mTransactionValidStart.getSeconds(),
					.nanos = mTransactionValidStart.getNanos(),
				},
				.accountID = {
				  .shardNum = mAccountId.getShardNum(),
					.realmNum = mAccountId.getRealmNum(),
					.accountNum = mAccountId.getAccountNum()
				}
			};

	    std::string result;
			result.reserve(grdw_hiero_transaction_id_calculate_string_size(&hieroTxId)+1);
			size_t written = grdw_hiero_transaction_id_to_string(result.data(), result.size(), &hieroTxId);
			if (written != result.size() + 1) {
			  printf("written: %zu, result size: %zu, result: %s\n", written, result.size(), result.c_str());
			  throw GradidoNodeInvalidDataException("error in hiero::TransactionId::toString()");
			}
	    return result;
	  }
}
