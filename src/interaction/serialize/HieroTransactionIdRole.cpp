#include "gradido_blockchain/interaction/serialize/HieroTransactionIdRole.h"
#include "gradido_blockchain/interaction/serialize/HieroAccountIdRole.h"
#include "gradido_blockchain/data/hiero/TransactionId.h"

namespace gradido {
	namespace interaction {
		namespace serialize {
			HieroTransactionIdMessage HieroTransactionIdRole::getMessage() const
			{
				HieroTransactionIdMessage message;
				auto transactionValidStart = mTransactionId.getTransactionValidStart();
				message["transactionValidStart"_f] = TimestampMessage{ transactionValidStart.getSeconds(), transactionValidStart.getNanos() };
				message["accountID"_f] = HieroAccountIdRole(mTransactionId.getAccountId()).getMessage();
				if (mTransactionId.isScheduled()) {
					message["scheduled"_f] = true;
				}
				if (mTransactionId.getNonce()) {
					message["nonce"_f] = mTransactionId.getNonce();
				}
				return message;
			}

			size_t HieroTransactionIdRole::calculateSerializedSize() const
			{
				size_t size = 4 + 9 + 5 + 5;
				size += HieroAccountIdRole(mTransactionId.getAccountId()).calculateSerializedSize();
				return size;
			}
		}
	}
}