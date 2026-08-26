#include "gradido_blockchain/data/hiero/TransactionId.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain_core/data/wire/hiero.h"
#include "gradido_blockchain/memory/Block.h"

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

      size_t bufferSize = grdw_hiero_transaction_id_calculate_string_size(&hieroTxId) + 1;
      if (bufferSize < 64) {
        char buffer[64]{};
        size_t written = grdw_hiero_transaction_id_to_string(buffer, 64, &hieroTxId);
        if (written >= 64) {
          throw GradidoNodeInvalidDataException("grdw_hiero_transaction_id_calculate_string_size and grdw_hiero_transaction_id_to_string don't calculate same string size");
        }
        return std::string(buffer, written);
      } 
      else if (bufferSize > 1024) {
        throw GradidoNodeInvalidDataException("hiero transaction id is calculated way to big (> 1 kbyte)");
      }
      else {
        memory::Block block(bufferSize);
        size_t written = grdw_hiero_transaction_id_to_string(reinterpret_cast<char*>(block.data()), bufferSize, &hieroTxId);
        return block.copyAsString();
      }
	  }
}
