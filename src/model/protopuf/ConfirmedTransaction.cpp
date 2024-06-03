#include "gradido_blockchain/model/protopuf/ConfirmedTransaction.h"

namespace model {
	namespace protopuf {
        ConfirmedTransaction::ConfirmedTransaction(const ConfirmedTransactionMessage& data)
            : mId(data["id"_f].value()),
            mGradidoTransaction(data["transaction"_f].value()),
            mConfirmedAt(data["confirmed_at"_f].value()),
            mVersionNumber(data["version_number"_f].value()),
            mRunningHash(std::make_shared<memory::Block>(data["running_hash"_f].value())),
            mMessageId(std::make_shared<memory::Block>(data["message_id"_f].value())),
            mAccountBalance(data["account_balance"_f].value_or("0"))
        {

        }
        ConfirmedTransaction::ConfirmedTransaction(
            uint64_t id,
            const GradidoTransaction& gradidoTransaction,
            Timepoint confirmedAt,
            const std::string& versionNumber,
            ConstMemoryBlockPtr runningHash,
            ConstMemoryBlockPtr messageId,
            const std::string& accountBalance
        )
            : mId(id),
            mGradidoTransaction(gradidoTransaction),
            mConfirmedAt(confirmedAt),
            mVersionNumber(versionNumber),
            mRunningHash(runningHash),
            mMessageId(messageId),
            mAccountBalance(accountBalance)
        {

        }
	}
}