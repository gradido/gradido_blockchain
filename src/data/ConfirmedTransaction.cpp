#include "gradido_blockchain_core/data/wire/confirmed_transaction.h"
#include "gradido_blockchain_core/memory.h"
#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/const.h"
#include "gradido_blockchain/data/adapter/accountBalance.h"
#include "gradido_blockchain/data/adapter/ledgerAnchor.h"
#include "gradido_blockchain/data/adapter/memoryBlock.h"
#include "gradido_blockchain/data/adapter/publicKey.h"
#include "gradido_blockchain/data/adapter/timestamp.h"
#include "gradido_blockchain/data/adapter/uuid.h"
#include "gradido_blockchain/data/compact/PublicKeyIndex.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/interaction/serialize/Context.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"
#include "gradido_blockchain/memory/Block.h"

#include "loguru/loguru.hpp"

#include <memory>
#include <optional>
#include <string>
#include <vector>

using DataTypeConverter::timePointToString;
using memory::Block, memory::ConstBlockPtr;
using std::optional;
using std::shared_ptr;
using std::string;
using std::vector;

namespace gradido {
	namespace data {
		using adapter::toPublicKeyIndex, adapter::uuidFromString, adapter::uuidToString;
		using namespace interaction;
		using compact::PublicKeyIndex;

		ConfirmedTransaction::ConfirmedTransaction(
			uint64_t id,
			shared_ptr<const GradidoTransaction> gradidoTransaction,
			Timestamp confirmedAt,
			const LedgerAnchor& ledgerAnchor,
			vector<AccountBalance> accountBalances,
			grdt_balance_derivation balanceDerivationType,
			shared_ptr<const ConfirmedTransaction> previousConfirmedTransaction/* = nullptr */
		) : mId(id),
			mGradidoTransaction(gradidoTransaction),
			mConfirmedAt(confirmedAt),
			mLedgerAnchor(ledgerAnchor),
			mAccountBalances(accountBalances),
			mBalanceDerivationType(balanceDerivationType)
		{
			mRunningHash = calculateRunningHash(previousConfirmedTransaction);
			initalizePubkeyHashes();
		}

		ConfirmedTransaction::ConfirmedTransaction(
			uint64_t id,
			shared_ptr<const GradidoTransaction> gradidoTransaction,
			Timestamp confirmedAt,
			ConstBlockPtr runningHash,
			const LedgerAnchor& ledgerAnchor,
			std::vector<AccountBalance> accountBalances,
			grdt_balance_derivation balanceDerivationType
		) : mId(id),
			mGradidoTransaction(gradidoTransaction),
			mConfirmedAt(confirmedAt),
			mRunningHash(runningHash),
			mLedgerAnchor(ledgerAnchor),
			mAccountBalances(accountBalances),
			mBalanceDerivationType(balanceDerivationType)
		{
			initalizePubkeyHashes();
		}

		shared_ptr<const ConfirmedTransaction> ConfirmedTransaction::fromGrdw(const grdw_confirmed_transaction* grdw_tx, uint32_t communityIdIndex)
		{
			std::vector<AccountBalance> accountBalances;
			if (grdw_tx->account_balances_count) {
				accountBalances.reserve(grdw_tx->account_balances_count);
				for (size_t i = 0; i < grdw_tx->account_balances_count; i++) {
					accountBalances.emplace_back(adapter::fromGrdw(grdw_tx->account_balances[i], communityIdIndex));
				}
			}

			return make_shared<const ConfirmedTransaction>(
				grdw_tx->id,
				GradidoTransaction::fromGrdw(&grdw_tx->transaction, communityIdIndex),
				adapter::fromGrdw(grdw_tx->confirmed_at),
				adapter::fromGrdw(grdw_tx->running_hash),
				adapter::fromGrdw(grdw_tx->ledger_anchor),
				accountBalances,
				grdw_tx->balance_derivation
			);
		}

		void ConfirmedTransaction::toGrdw(grd_memory* alloc, grdw_confirmed_transaction* grdw_tx, uint32_t communityIdIndex) const
		{
			assert(mGradidoTransaction);
			grdw_tx->id = mId;
			mGradidoTransaction->toGrdw(alloc, &grdw_tx->transaction, communityIdIndex);
			grdw_tx->confirmed_at = adapter::toGrdw(mConfirmedAt);
			memcpy(grdw_tx->running_hash, mRunningHash->data(), mRunningHash->size());
			grdw_tx->ledger_anchor = adapter::toGrdw(alloc, mLedgerAnchor);
			if (mAccountBalances.size()) {
				grdw_confirmed_transaction_reserve_account_balances(grdw_tx, mAccountBalances.size(), alloc);
				if (grdw_tx->account_balances) {
					for (int i = 0; i < mAccountBalances.size(); i++) {
						grdw_tx->account_balances[i] = adapter::toGrdw(alloc, mAccountBalances[i], communityIdIndex);
					}
				}
			}
			grdw_tx->balance_derivation = mBalanceDerivationType;
		}

		ConstBlockPtr ConfirmedTransaction::calculateRunningHash(
			shared_ptr<const ConfirmedTransaction> previousConfirmedTransaction/* = nullptr*/
		) const {
			string transactionIdString = std::to_string(mId);
			auto confirmedAtString = mConfirmedAt.toString();// timePointToString(mConfirmedAt, "%Y-%m-%d %H:%M:%S");
			auto ledgerAnchorString = mLedgerAnchor.toString();
			auto hash = make_shared<Block>(crypto_generichash_BYTES);

			// Sodium use for the generic hash function BLAKE2b today (11.11.2019), maybe change in the future
			crypto_generichash_state state;
			crypto_generichash_init(&state, nullptr, 0, crypto_generichash_BYTES);
			if (previousConfirmedTransaction && previousConfirmedTransaction->mRunningHash) {
				auto prevHashHex = previousConfirmedTransaction->mRunningHash->convertToHex();
				crypto_generichash_update(&state, (const unsigned char*)prevHashHex.data(), prevHashHex.size());
			}
			crypto_generichash_update(&state, (const unsigned char*)transactionIdString.data(), transactionIdString.size());

			crypto_generichash_update(&state, (const unsigned char*)confirmedAtString.data(), confirmedAtString.size());

			crypto_generichash_update(&state, (const unsigned char*)ledgerAnchorString.data(), ledgerAnchorString.size());

			if (mGradidoTransaction->getSignatureMap().getSignaturePairs().size()) {
				const auto& sigPairs = mGradidoTransaction->getSignatureMap().getSignaturePairs();
				for (const auto& sigPair : sigPairs) {
					crypto_generichash_update(&state, sigPair.getPublicKey()->data(), 32);
					crypto_generichash_update(&state, sigPair.getSignature()->data(), 64);
				}
			}
			for (auto& accountBalance : mAccountBalances) {
				auto gdd = accountBalance.getBalance().getGradidoCent();
				crypto_generichash_update(&state, (const unsigned char*)&gdd, sizeof(gdd));
			}
			crypto_generichash_update(&state, (const unsigned char*)&mBalanceDerivationType, 1);
			crypto_generichash_final(&state, hash->data(), hash->size());
			return hash;
		}

		bool ConfirmedTransaction::hasAccountBalance(const Block& publicKey, optional<uint32_t> communityIdIndex) const
		{
			for (auto& accountBalance : mAccountBalances) {
				{
					if (accountBalance.belongsTo(publicKey, communityIdIndex)) {
						return true;
					}
				}
			}
			return false;
		}

		AccountBalance ConfirmedTransaction::getAccountBalance(memory::ConstBlockPtr publicKey, optional<uint32_t> communityIdIndex) const
		{
			for (auto& accountBalance : mAccountBalances) {
				if (accountBalance.belongsTo(*publicKey, communityIdIndex)) {
					return accountBalance;
				}
			}
			return AccountBalance(publicKey, GradidoUnit::zero(), mGradidoTransaction->getCommunityIdIndex());
		}

		AccountBalance ConfirmedTransaction::getAccountBalance(memory::ConstBlockPtr publicKey, const std::string& communityId) const
		{
			return getAccountBalance(publicKey, uuidFromString(communityId.c_str()));
		}

		AccountBalance ConfirmedTransaction::getAccountBalance(memory::ConstBlockPtr publicKey, const Uuid& communityUuid) const
		{
			auto communityIdIndex = g_appContext->getCommunityIds().getIndexForData(communityUuid);
			if (communityIdIndex) {
				return getAccountBalance(publicKey, static_cast<uint32_t>(communityIdIndex));
			}
			else {
				LOG_F(WARNING, "community id index not found for: %s", uuidToString(communityUuid).c_str());
			}
			return AccountBalance(publicKey, GradidoUnit::zero(), mGradidoTransaction->getCommunityIdIndex());
		}

		bool ConfirmedTransaction::isInvolved(const Block& publicKey) const
		{
			return isInvolved(toPublicKeyIndex(publicKey, getGradidoTransaction()->getCommunityIdIndex()));
		}

		bool ConfirmedTransaction::isInvolved(const compact::PublicKeyIndex publicKeyIndex) const
		{
			if (mPubkeyIndices.size()) {
				for (auto& pubIdx : mPubkeyIndices) {
					if (publicKeyIndex == pubIdx) {
						return true;
					}
				}
			}
			else {
				if (isBalanceUpdated(publicKeyIndex)) {
					return true;
				}
				return getGradidoTransaction()->isInvolved(publicKeyIndex);
			}
			return false;
		}

		bool ConfirmedTransaction::isBalanceUpdated(const Block& publicKey) const
		{
			for (auto& accountBalance : mAccountBalances) {
				if (accountBalance.getPublicKey()->isTheSame(publicKey)) {
					return true;
				}
			}
			return false;
		}

		bool ConfirmedTransaction::isBalanceUpdated(const compact::PublicKeyIndex publicKeyIndex) const
		{
			auto communityIdIndex = getGradidoTransaction()->getCommunityIdIndex();
			for (auto& accountBalance : mAccountBalances) {
				auto accountBalancePublicKeyIndex = toPublicKeyIndex(accountBalance.getPublicKey(), communityIdIndex);
				if (accountBalancePublicKeyIndex == publicKeyIndex) {
					return true;
				}
			}
			return false;
		}

		vector<ConstBlockPtr> ConfirmedTransaction::getInvolvedAddresses() const
		{
			auto involvedAddresses = getGradidoTransaction()->getInvolvedAddresses();
			for (auto& accountBalance : mAccountBalances) {
				bool found = false;
				for (auto& involvedAddress : involvedAddresses) {
					if (involvedAddress->isTheSame(accountBalance.getPublicKey())) {
						found = true;
						break;
					}
				}
				if (!found) {
					involvedAddresses.push_back(accountBalance.getPublicKey());
				}
			}
			return involvedAddresses;
		}

		vector<PublicKeyIndex> ConfirmedTransaction::getInvolvedAddressIndices() const
		{
			auto involvedAddresses = getGradidoTransaction()->getInvolvedAddressIndices();
			auto communityIdIndex = getGradidoTransaction()->getCommunityIdIndex();
			for (auto& accountBalance : mAccountBalances) {
				auto publicKeyIndex = toPublicKeyIndex(accountBalance.getPublicKey(), communityIdIndex);
				bool found = false;
				for (auto& involvedAddress : involvedAddresses) {
					if (publicKeyIndex == involvedAddress) {
						found = true;
						break;
					}
				}
				if (!found) {
					involvedAddresses.emplace_back(publicKeyIndex);
				}
			}
			return involvedAddresses;
		}

		bool ConfirmedTransaction::isTheSame(const ConfirmedTransaction& other) const
		{
			if (mRunningHash && other.mRunningHash) {
				return mRunningHash->isTheSame(other.mRunningHash);
			}
			LOG_F(WARNING, "missing running hash from confirmed transaction, make full comparisation");

			if (mId != other.mId) {
				return false;
			}
			if (!mGradidoTransaction || !other.mGradidoTransaction) {
				LOG_F(WARNING, "missing gradido transaction for compare confirmed transaction");
				if (!(!mGradidoTransaction && !other.mGradidoTransaction)) {
					return false;
				}
			}
			else if (!mGradidoTransaction->isTheSame(*other.mGradidoTransaction)) {
				return false;
			}

			if (mConfirmedAt != other.mConfirmedAt) {
				return false;
			}

			if (!mLedgerAnchor.isTheSame(mLedgerAnchor)) {
				return false;
			}
			if (mAccountBalances.size() != other.mAccountBalances.size()) {
				return false;
			}
			if (mBalanceDerivationType != other.mBalanceDerivationType) {
				return false;
			}

			for (size_t i = 0; i < mAccountBalances.size(); i++) {
				if (!mAccountBalances[i].isTheSame(other.mAccountBalances[i])) {
					LOG_F(WARNING, "assume same account balance order, consider this result as instable");
					return false;
				}
			}
			return true;
		}

		void ConfirmedTransaction::initalizePubkeyHashes()
		{
			auto involvedAddresses = getInvolvedAddressIndices();
			mPubkeyIndices.reserve(involvedAddresses.size());
			for (auto& address : involvedAddresses) {
				mPubkeyIndices.emplace_back(address);
			}
		}
	}
}