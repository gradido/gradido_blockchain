#ifndef GRADIDO_BLOCKCHAIN_DATA_RUNTIME_COMPLETE_TRANSACTION_H
#define GRADIDO_BLOCKCHAIN_DATA_RUNTIME_COMPLETE_TRANSACTION_H

#include "gradido_blockchain/data/AccountBalance.h"
#include "gradido_blockchain/data/ByteArray.h"
#include "gradido_blockchain/data/EncryptedMemo.h"
#include "gradido_blockchain/data/LedgerAnchor.h"
#include "gradido_blockchain/data/Timestamp.h"
#include "gradido_blockchain/export.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain_core/data/runtime/complete_transaction.h"
#include "gradido_blockchain_core/data/wire/basic_types.h"
#include "gradido_blockchain_core/result.h"

#include <optional>
#include <span>
#include <vector>

struct grdw_account_balance;
struct grdw_confirmed_transaction;
struct grdw_ledger_anchor;
struct grdw_transaction_body;
struct grdw_transfer_amount;

namespace gradido::data::runtime {
	class GRADIDOBLOCKCHAIN_EXPORT CompleteTransaction: protected grdr_complete_transaction
	{
	public:
		CompleteTransaction();
		virtual ~CompleteTransaction();

		// copy
		CompleteTransaction(const CompleteTransaction& other) = delete;
		// move
		CompleteTransaction(CompleteTransaction&& other) = delete;

		grd_result initFromGrdw(
			const grdw_transaction_body* body,
			const grdw_confirmed_transaction* confirmedTx,
			Uuid::ConstViewType communityUuid
		);
		grd_result initFromProtobuf(const grd_memory_block& inputBuffer, Uuid::ConstViewType communityUuid);
		inline grd_result initFromProtobuf(const memory::Block& serializedConfirmedTx, Uuid::ConstViewType communityUuid) {
			grd_memory_block src = { .data = (uint8_t*)serializedConfirmedTx.data(), .size = serializedConfirmedTx.size() };
			return initFromProtobuf(src, communityUuid);
		};

		grd_result validate(bool verifySignatures = true);

		// getter
		// confirmed tx
		inline uint64_t getId() const { return  tx_nr; }
		inline Timestamp getConfirmedAt() const { return confirmed_at; }
		inline GenericHash getRunningHash() const { return tx_running_hash; }
		inline LedgerAnchor getLedgerAnchor() const { return ledger_anchor; }
		std::vector<grdw_account_balance> getAccountBalances() const;

		//! \param coinCommunityUuid optional
		//! \return accountBalance if found one with same public key or nullptr
		inline const grdw_account_balance* getAccountBalance(
		  PublicKey::ConstViewType publicKey,
			std::optional<Uuid::ConstViewType> coinCommunityUuid = std::nullopt
		) const;

		//! \return 0 if account balance couldn't be find
		inline GradidoUnit getDecayedAccountBalance(
			PublicKey::ConstViewType publicKey,
			std::optional<Uuid::ConstViewType> coinCommunityUuid = std::nullopt,
			Timepoint endDate = std::chrono::system_clock::now()
		) const;
		grdt_balance_derivation getBalanceDerivationType() const { return balance_derivation_type; }
		bool isBalanceNodeComputed() const { return GRDT_BALANCE_DERIVATION_NODE == balance_derivation_type; }
		bool isBalanceExternComputed() const { return GRDT_BALANCE_DERIVATION_EXTERN == balance_derivation_type; }
		bool isInvolved(PublicKey::ConstViewType publicKey) const;
		bool isBalanceUpdated(PublicKey::ConstViewType publicKey) const;
		std::vector<PublicKey> getInvolvedAddresses() const;
		// bool isTheSame(const CompleteTransaction& other) const;

		// gradido transaction
		std::vector<grdw_signature_pair> getSignatureMap();
		inline memory::Block getBodyBytes() const { return memory::Block(body_bytes.size, body_bytes.data); }
		inline Uuid::ConstViewType getCommunityUuid() const { return Uuid::ConstViewType(tx_community_uuid, 16); }
		inline std::optional<LedgerAnchor> getPairingLedgerAnchor() const;

		// transaction body

		inline bool isTransfer() const { return GRDT_TRANSACTION_TRANSFER == transaction_type; }
		inline bool isCreation() const { return GRDT_TRANSACTION_CREATION == transaction_type; }
		inline bool isCommunityFriendsUpdate() const { return GRDT_TRANSACTION_COMMUNITY_FRIENDS_UPDATE == transaction_type; }
		inline bool isRegisterAddress() const { return GRDT_TRANSACTION_REGISTER_ADDRESS == transaction_type; }
		inline bool isDeferredTransfer() const { return GRDT_TRANSACTION_DEFERRED_TRANSFER == transaction_type; }
		inline bool isCommunityRoot() const { return GRDT_TRANSACTION_COMMUNITY_ROOT == transaction_type; }
		inline bool isRedeemDeferredTransfer() const { return GRDT_TRANSACTION_REDEEM_DEFERRED_TRANSFER == transaction_type; }
		inline bool isTimeoutDeferredTransfer() const { return GRDT_TRANSACTION_TIMEOUT_DEFERRED_TRANSFER == transaction_type; }
		inline grdt_transaction getTransactionType() const { return transaction_type; }

		inline std::optional<GradidoUnit> getAmount() const;
		inline std::optional<Uuid::ConstViewType> getCoinCommunityUuid() const;
		inline bool hasTransferAmount() const { return isTransfer() || isRedeemDeferredTransfer() || isDeferredTransfer() || isCreation(); }
		inline std::vector<EncryptedMemo> getMemos() const;
		inline Timestamp getCreatedAt() const { return created_at; }
		inline grdt_cross_group getCrossGroupType() const { return cross_group_type; }
		inline std::optional<Uuid::ConstViewType> getOtherCommunityUuid() const;
		// full public key
		//! get sender public key index if it transfer or deferred transfer transaction else std::nullopt
		inline std::optional<PublicKey::ConstViewType> getSenderPublicKey() const;
		inline std::optional<Uuid::ConstViewType> getSenderCommunityUuid() const;
		//! get recipient public key index if it is creation, transfer or deferred transfer transaction else std::nullopt
		inline std::optional<PublicKey::ConstViewType> getRecipientPublicKey() const;
		inline std::optional<Uuid::ConstViewType> getRecipientCommunityUuid() const;
		//! get user public key on register address transaction else std::nullopt
		inline std::optional<PublicKey::ConstViewType> getRegisteredUser() const;
		//! get account public key on register address transaction else std::nullopt
		inline std::optional<PublicKey::ConstViewType> getRegisteredAccount() const;
		inline std::optional<grdt_address> getRegisteredAddressType() const;
		inline std::optional<uint32_t> getRegisteredDerivationIndex() const;
		inline std::optional<GenericHash::ConstViewType> getRegisteredNameHash() const;
		//! get root public key on community root transaction else std::nullopt
		inline std::optional<PublicKey::ConstViewType> getCommunityRootPublicKey() const;
		//! get auf account public key on community root transaction else std::nullopt
		inline std::optional<PublicKey::ConstViewType> getAuf() const;
		//! get gmw account public key on community root transaction else std::nullopt
		inline std::optional<PublicKey::ConstViewType> getGmw() const;

		inline std::optional<grdd_timestamp_seconds> getCreationTargetDate() const;
		inline std::optional<grdd_duration_seconds> getDeferredTransferTimeoutDuration() const;
		// only for transaction types GRDT_TRANSACTION_REDEEM_DEFERRED_TRANSFER and GRDT_TRANSACTION_TIMEOUT_DEFERRED_TRANSFER
		inline std::optional<uint64_t> getPreviousTx() const;

	protected:
	};

	const grdw_account_balance* CompleteTransaction::getAccountBalance(
	  PublicKey::ConstViewType publicKey,
		std::optional<Uuid::ConstViewType> coinCommunityUuid /* = std::nullopt */
	) const {
	  const grdw_account_balance* account_balance = grdr_complete_transaction_get_account_balance_for_public_key(this, publicKey.data());
		if (coinCommunityUuid.has_value() && account_balance &&
		  !isTheSame(coinCommunityUuid.value(), Uuid::ConstViewType(account_balance->community_uuid, 16))
		) {
		  return nullptr;
		}
		return account_balance;
	}

	GradidoUnit CompleteTransaction::getDecayedAccountBalance(
		PublicKey::ConstViewType publicKey,
		std::optional<Uuid::ConstViewType> coinCommunityUuid /* = std::nullopt */,
		Timepoint endDate/* = std::chrono::system_clock::now()*/
	) const {
	  const grdw_account_balance* account_balance = getAccountBalance(publicKey, coinCommunityUuid);
		if (!account_balance) return GradidoUnit::zero();
		return AccountBalance(account_balance)
			.getBalance()
			.calculateDecay(Timestamp(confirmed_at).getAsTimepoint(), endDate);
	}

	std::optional<LedgerAnchor> CompleteTransaction::getPairingLedgerAnchor() const {
		if (!pairing_ledger_anchor) {
			return std::nullopt;
		}
		return *pairing_ledger_anchor;
	}

	std::optional<GradidoUnit> CompleteTransaction::getAmount() const {
		if (hasTransferAmount()) {
			return GradidoUnit::fromGradidoCent(transfer.amount);
		}
		return std::nullopt;
	}

	std::optional<Uuid::ConstViewType> CompleteTransaction::getCoinCommunityUuid() const {
		if (hasTransferAmount()) {
			return transfer.coin_community_uuid;
		}
		return std::nullopt;
	}

	std::optional<Uuid::ConstViewType> CompleteTransaction::getOtherCommunityUuid() const {
		if (tx_pairing_community_uuid) {
			return Uuid::ConstViewType(tx_pairing_community_uuid, 16);
		}
		return std::nullopt;
	}

	std::optional<PublicKey::ConstViewType> CompleteTransaction::getSenderPublicKey() const {
		if (isTransfer() || isRedeemDeferredTransfer() || isDeferredTransfer()) {
			return transfer.sender_pubkey;
		}
		return std::nullopt;
	}

	std::optional<Uuid::ConstViewType> CompleteTransaction::getSenderCommunityUuid() const {
	  const uint8_t* uuid_ptr = grdr_complete_transaction_get_sender_community_uuid(this);
		if (uuid_ptr) { return Uuid::ConstViewType(uuid_ptr, 16); }
		return std::nullopt;
	}

	std::optional<PublicKey::ConstViewType> CompleteTransaction::getRecipientPublicKey() const {
		if (isTransfer() || isRedeemDeferredTransfer() || isDeferredTransfer() || isCreation()) {
			return transfer.recipient_pubkey;
		}
		return std::nullopt;
	}

	std::optional<Uuid::ConstViewType> CompleteTransaction::getRecipientCommunityUuid() const
	{
	  const uint8_t* uuid_ptr = grdr_complete_transaction_get_recipient_community_uuid(this);
		if (uuid_ptr) { return Uuid::ConstViewType(uuid_ptr, 16); }
		return std::nullopt;
	}

	std::optional<PublicKey::ConstViewType> CompleteTransaction::getRegisteredUser() const {
		if (!isRegisterAddress()) { return std::nullopt; }
		return register_address.user_public_key;
	}

	std::optional<PublicKey::ConstViewType> CompleteTransaction::getRegisteredAccount() const {
		if (!isRegisterAddress()) { return std::nullopt; }
		return register_address.account_public_key;
	}

	std::optional<grdt_address> CompleteTransaction::getRegisteredAddressType() const {
		if (!isRegisterAddress()) { return std::nullopt; }
		return address_type;
	}

	std::optional<uint32_t> CompleteTransaction::getRegisteredDerivationIndex() const {
		if (!isRegisterAddress()) { return std::nullopt; }
		return derivation_index;
	}

	std::optional<GenericHash::ConstViewType> CompleteTransaction::getRegisteredNameHash() const
	{
		if (!isRegisterAddress()) { return std::nullopt; }
		return register_address.name_hash;
	}

	std::optional<PublicKey::ConstViewType> CompleteTransaction::getCommunityRootPublicKey() const {
		if (!isCommunityRoot()) { return std::nullopt; }
		return community_root.public_key;
	}

	std::optional<PublicKey::ConstViewType> CompleteTransaction::getAuf() const {
		if (!isCommunityRoot()) { return std::nullopt; }
		return community_root.auf_public_key;
	}

	std::optional<PublicKey::ConstViewType> CompleteTransaction::getGmw() const {
		if (!isCommunityRoot()) { return std::nullopt; }
		return community_root.gmw_public_key;
	}

	std::optional<grdd_timestamp_seconds> CompleteTransaction::getCreationTargetDate() const {
		if (isCreation()) {
			return target_date;
		}
		return std::nullopt;
	}

	std::optional<grdd_duration_seconds> CompleteTransaction::getDeferredTransferTimeoutDuration() const {
		if (isDeferredTransfer()) {
			return timeout_duration;
		}
		return std::nullopt;
	}

	// only for transaction types GRDT_TRANSACTION_REDEEM_DEFERRED_TRANSFER and GRDT_TRANSACTION_TIMEOUT_DEFERRED_TRANSFER
	std::optional<uint64_t> CompleteTransaction::getPreviousTx() const {
		if (isRedeemDeferredTransfer() || isTimeoutDeferredTransfer()) {
			return previous_tx;
		}
		return std::nullopt;
	}
}

#endif //GRADIDO_BLOCKCHAIN_DATA_RUNTIME_COMPLETE_TRANSACTION_H
