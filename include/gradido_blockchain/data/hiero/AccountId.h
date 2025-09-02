#ifndef __GRADIDO_BLOCKCHAIN_DATA_HIERO_ACCOUNT_ID
#define __GRADIDO_BLOCKCHAIN_DATA_HIERO_ACCOUNT_ID

/**
 * A unique identifier for an Hedera account.
 *
 * An account identifier is of the form `shard.realm.[number|alias]`.<br/>
 * The identifier MAY use the alias form when transferring HBAR to a public key
 * before the account for that key is created, when only the alias value is
 * known, or in some smart contracts that use the EVM address style alias to
 * refer to Accounts.<br/>
 * When the account entry is completed, the alias SHALL be stored separately in
 * the Account record, and the identifier in the Account SHALL use the
 * `accountNum` form.
 *
 * ---
 * ### Additional Notes
 *
 * #### Alias
 * There is considerable complexity with `alias` (aka `evm_address`) for
 * Accounts. Much of this comes from the existence of a "hidden" alias for
 * almost all accounts, and the reuse of the alias field for both EVM reference
 * and "automatic" account creation.<br/>
 * For the purposes of this specification, we will use the following terms for
 * clarity.
 *   - `key_alias`<br/>
 *      The account public key as a protobuf serialized message and used for
 *      auto-creation and subsequent lookup. This is only valid if the account
 *      key is a single `primitive` key, either Ed25519 or ECDSA_SECP256K1.
 *   - `evm_address`<br/>
 *     Exists for every account and is one of
 *      - `contract_address`<br/>
 *        The 20 byte EVM address prescribed by `CREATE` or `CREATE2`
 *      - `evm_key_address`<br/>
 *        An arbitrary 20 byte EVM address that, for a usable externally owned
 *        account (EOA) SHALL be the rightmost 20 bytes of the Keccak-256 hash
 *        of a ECDSA_SECP256K1 key.<br/>
 *        Such accounts may be created in one of three ways:
 *        - Sending hbar or fungible tokens to an unused
 *          ECDSA_SECP256K1 key alias.
 *        - Sending hbar or fungible tokens to an unassigned 20-byte
 *          EVM address.
 *        - Submitting a `CryptoCreate` signed with the corresponding
 *          private key.
 *      - `long_zero`<br/>
 *        A synthetic 20 byte address inferred for "normally" created accounts.
 *        It is constructed from the "standard" AccountID as follows.
 *         1. 4 byte big-endian shard number
 *         1. 8 byte big-endian realm number
 *         1. 8 byte big-endian entity number<br/>
 *
 * The `alias` field in the `Account` message SHALL contain one of four values
 * for any given account.
 *   - The `key_alias`, if the account was created by transferring HBAR to the
 *     `key_alias` public key value.
 *   - The `evm_key_address` if the account was created from an EVM public key
 *   - The `contract_address` if the account belongs to an EVM contract
 *   - Not-Set/null/Bytes.EMPTY (collectively `null`) if the account was
 *     created normally
 *
 * If the `alias` field of an `Account` is any form of `null`, then the account
 * MAY be referred to by `alias` in an `AccountID` by using the `long_zero`
 * address for the account.<br/>
 * This "hidden default" alias SHALL NOT be stored, but is synthesized by the
 * node software as needed, and may be synthesized by an EVM contract or client
 * software as well.
 *
 * ---
 *
 * #### Alias forms
 * An `AccountID` in a transaction MAY reference an `Account` with
 * `shard.realm.alias`.<br/>
 * If the account `alias` field is set for an Account, that value SHALL be the
 * account alias.<br/>
 * If the account `alias` field is not set for an Account, the `long_zero` alias
 * SHALL be the account alias.
 */

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/types.h"
#include "gradido_blockchain/memory/Block.h"

namespace hiero {
	class GRADIDOBLOCKCHAIN_EXPORT AccountId
	{
	public:
		AccountId();
		AccountId(int64_t shardNum, int64_t realmNum, int64_t accountNum);
		// copy alias
		AccountId(int64_t shardNum, int64_t realmNum, const memory::Block& alias);
		// move alias
		AccountId(int64_t shardNum, int64_t realmNum, memory::Block&& alias);
		//! expect string like 0.0.2
		//! can also parse string like 0.0.2-1321201-212122 for example a transaction id, will ignore everything after the third number
		AccountId(const std::string& accountIdString);
		~AccountId();

		// default constructor and operators
		AccountId(const AccountId& other) = default;
		AccountId(AccountId&& other) noexcept = default;
		AccountId& operator=(const AccountId& other) = default;
		AccountId& operator=(AccountId&& other) noexcept = default;

		int64_t getShardNum() const { return mShardNum;  }
		int64_t getRealmNum() const { return mRealmNum;  }
		int64_t getAccountNum() const { return mAccountNum; }
		const memory::Block& getAlias() const { return mAlias; }

		std::string toString() const;
		inline bool operator==(const AccountId& other) const;
		
	protected:
		int64_t mShardNum;
		int64_t mRealmNum;
		int64_t mAccountNum; 
		memory::Block mAlias;
	};

	inline bool AccountId::operator==(const AccountId& other) const
	{
		return mShardNum == other.mShardNum
			&& mRealmNum == other.mRealmNum
			&& mAccountNum == other.mAccountNum
			&& mAlias.isTheSame(other.mAlias)
		;
	}

	inline bool operator<(AccountId const& lhs, AccountId const& rhs)
	{
		if (lhs.getShardNum() != rhs.getShardNum()) {
			return lhs.getShardNum() < rhs.getShardNum();
		}
		if (lhs.getRealmNum() != rhs.getRealmNum()) {
			return lhs.getRealmNum() < rhs.getRealmNum();
		}
		if (lhs.getAccountNum() != rhs.getAccountNum()) {
			return lhs.getAccountNum() < rhs.getAccountNum();
		}
		return lhs.getAlias() < rhs.getAlias();
	}

	
}

#endif // __GRADIDO_BLOCKCHAIN_DATA_HIERO_ACCOUNT_ID