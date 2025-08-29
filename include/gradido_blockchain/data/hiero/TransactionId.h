#ifndef __GRADIDO_BLOCKCHAIN_DATA_HIERO_TRANSACTION_ID
#define __GRADIDO_BLOCKCHAIN_DATA_HIERO_TRANSACTION_ID

/**
 * A transaction identifier.<br/>
 * This is used for retrieving receipts and records for a transaction
 * and internally by the network for detecting when duplicate transactions are
 * submitted.
 *
 * A transaction may be processed more reliably by submitting it to
 * several nodes, each with a different node account, but all with the same
 * TransactionID. Then, the transaction will take effect when the first of all
 * those nodes submits the transaction and it reaches consensus. The other
 * transactions SHALL NOT be executed (and SHALL result in a
 * `DUPLICATE_TRANSACTION` response).<br/>
 * Multiple submission increase reliability on the assumption that an error in,
 * for example, network connectivity will not affect all nodes equally. Latency
 * might be slightly lower, if one node is handling intake significantly slower
 * than others, for example. The base transaction fee is required for each
 * submission, however, so the total fees charged are significantly higher when
 * using this approach.
 *
 * ### Requirements
 * Each transaction identifier MUST be unique.<br/>
 * Multiple transactions MAY be submitted with the same transaction
 * identifier, but all except the first SHALL be rejected as duplicate
 * transactions.<br/>
 * An identifier MUST specify a `payer` account to be charged all fees
 * associated with the transaction.<br/>
 * The `payer` account MUST exist and MUST have sufficient HBAR to pay all
 * transaction fees.<br/>
 * An identifier MUST specify a "valid start time".<br/>
 * The "valid start time" MUST be strictly _earlier_ than the current
 * network consensus time when submitted.<br/>
 * The "valid start time" MUST NOT be more than `transaction.maxValidDuration`
 * seconds before the current network consensus time when submitted.<br/>
 * A client-submitted transaction MUST NOT set the `scheduled` flag.
 *
 * ### Additional Notes
 *
 * Additional items applicable to Scheduled Transactions:
 *
 *  - The ID of a Scheduled Transaction, once executed, SHALL inherit both
 *    `transactionValidStart` and `accountID` from the `ScheduleCreate`
 *    transaction that created the schedule.
 *  - The `scheduled` property SHALL be set for Scheduled Transactions.
 */


#include "gradido_blockchain/export.h"
#include "gradido_blockchain/types.h"
#include "gradido_blockchain/data/Timestamp.h"
#include "gradido_blockchain/data/hiero/AccountId.h"

namespace hiero {
	class GRADIDOBLOCKCHAIN_EXPORT TransactionId
	{
	public: 
		TransactionId();
		TransactionId(const gradido::data::Timestamp& transactionValidStart, const AccountId& accountId);
		//! expect string in format 0.0.2-17281772-21022 or 0.0.256009@1755503343.736000193
		//! sharedNum.realmNum.accountNum-seconds-nanoseconds
		TransactionId(const std::string& transactionIdString);
		~TransactionId();
		inline void setScheduled();
		inline void setNonce(int32_t nonce);

		inline const gradido::data::Timestamp& getTransactionValidStart() const { return mTransactionValidStart; }
		inline const AccountId& getAccountId() const { return mAccountId; }
		inline bool isScheduled() const { return mScheduled; }
		inline int32_t getNonce() const { return mNonce; }

		//! format in hedera block explorer format: 0.0.256009@1755503343.736000193
		//! ignore scheduled and nonce completly
		std::string toString() const;

		inline bool empty() const { return mAccountId.empty() && mTransactionValidStart.empty(); }

	protected:
		gradido::data::Timestamp mTransactionValidStart;
		AccountId mAccountId;
		bool mScheduled;
		int32_t mNonce;
	};


	// --------------------------------  inline implementations ------------------------------------------------
	void TransactionId::setScheduled() {
		mScheduled = true;
	}

	void TransactionId::setNonce(int32_t nonce) {
		mNonce = nonce;
	}
}

#endif //__GRADIDO_BLOCKCHAIN_DATA_HIERO_TRANSACTION_ID