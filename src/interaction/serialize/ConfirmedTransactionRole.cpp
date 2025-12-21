#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/interaction/serialize/ConfirmedTransactionRole.h"
#include "gradido_blockchain/interaction/serialize/HieroTransactionIdRole.h"
#include "gradido_blockchain/interaction/serialize/LedgerAnchorRole.h"
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
				std::vector<AccountBalanceMessage> accountBalanceMessages;
				accountBalanceMessages.reserve(mConfirmedTransaction.getAccountBalances().size());
				for (auto& accountBalance : mConfirmedTransaction.getAccountBalances()) {
					AccountBalanceMessage accountBalanceMessage;
					accountBalanceMessage["pubkey"_f] = accountBalance.getPublicKey()->copyAsVector();
					accountBalanceMessage["balance"_f] = accountBalance.getBalance().getGradidoCent();
					if (!accountBalance.getCommunityId().empty()) {
						accountBalanceMessage["community_id"_f] = accountBalance.getCommunityId();
					}
					accountBalanceMessages.push_back(accountBalanceMessage);
				}			
				ConfirmedTransactionMessage message;
				message["id"_f] = mConfirmedTransaction.getId();
				message["transaction"_f] = mGradidoTransactionRole.getMessage();
				message["confirmed_at"_f] = TimestampMessage{
						mConfirmedTransaction.getConfirmedAt().getSeconds(),
						mConfirmedTransaction.getConfirmedAt().getNanos()
				};
				message["version_number"_f] = mConfirmedTransaction.getVersionNumber();
				message["running_hash"_f] = mConfirmedTransaction.getRunningHash()->copyAsVector();
				LedgerAnchorRole ledgerAnchorRole(mConfirmedTransaction.getLedgerAnchor());
				const auto& ledgerAnchor = mConfirmedTransaction.getLedgerAnchor();
				message["ledger_anchor"_f] = ledgerAnchorRole.getMessage();
				message["account_balances"_f] = accountBalanceMessages;
				message["balance_derivation"_f] = mConfirmedTransaction.getBalanceDerivationType();
				
				return message;
			}

			size_t ConfirmedTransactionRole::calculateSerializedSize() const
			{
				size_t size = 8 + 8 + 4 
					+ mConfirmedTransaction.getVersionNumber().size()
					+ crypto_generichash_BYTES 
					+ 32 
					+ mConfirmedTransaction.getAccountBalances().size() * 42
					+ 12;
				size += mGradidoTransactionRole.calculateSerializedSize();
				size += LedgerAnchorRole::serializedSize(mConfirmedTransaction.getLedgerAnchor());
				//printf("calculated confirmed transaction size: %lld\n", size);
				return size;
			}

		}
	}
}
