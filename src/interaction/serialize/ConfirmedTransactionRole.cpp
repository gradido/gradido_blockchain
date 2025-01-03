#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/interaction/serialize/ConfirmedTransactionRole.h"
#include "gradido_blockchain/interaction/serialize/Exceptions.h"

namespace gradido {
	namespace interaction {
		namespace serialize {

			ConfirmedTransactionRole::ConfirmedTransactionRole(const data::ConfirmedTransaction& confirmedTransaction)
				: mConfirmedTransaction(confirmedTransaction), mGradidoTransactionRole(*mConfirmedTransaction.getGradidoTransaction()) 
			{
			}

			ConfirmedTransactionRole::~ConfirmedTransactionRole() 
			{
			}

			ConfirmedTransactionMessage ConfirmedTransactionRole::getMessage() const
			{
				if (!mConfirmedTransaction.getRunningHash()) {
					throw MissingMemberException("missing member by serializing ConfirmedTransaction", "runningHash");
				}
				if (!mConfirmedTransaction.getMessageId()) {
					throw MissingMemberException("missing member by serializing ConfirmedTransaction", "messageId");
				}
				std::vector<AccountBalanceMessage> accountBalanceMessages;
				accountBalanceMessages.reserve(mConfirmedTransaction.getAccountBalances().size());
				for (auto& accountBalance : mConfirmedTransaction.getAccountBalances()) {
					accountBalanceMessages.push_back(
						AccountBalanceMessage(
							accountBalance.getPublicKey()->copyAsVector(),
							accountBalance.getBalance().getGradidoCent()
						)
					);
				}				
				return ConfirmedTransactionMessage{
					mConfirmedTransaction.getId(),
					mGradidoTransactionRole.getMessage(),
					TimestampSecondsMessage { mConfirmedTransaction.getConfirmedAt().getSeconds()},
					mConfirmedTransaction.getVersionNumber(),
					mConfirmedTransaction.getRunningHash()->copyAsVector(),
					mConfirmedTransaction.getMessageId()->copyAsVector(),
					accountBalanceMessages
				};
				// return confirmedTransactionMessage;
			}

			size_t ConfirmedTransactionRole::calculateSerializedSize() const
			{
				size_t size = 8 + 8 
					+ mConfirmedTransaction.getVersionNumber().size()
					+ crypto_generichash_BYTES 
					+ 32 
					+ mConfirmedTransaction.getAccountBalances().size() * 42
					+ 10;
				size += mGradidoTransactionRole.calculateSerializedSize();
				//printf("calculated confirmed transaction size: %lld\n", size);
				return size;
			}

		}
	}
}
