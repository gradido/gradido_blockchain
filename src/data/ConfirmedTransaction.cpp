#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"
#include "gradido_blockchain/interaction/serialize/Context.h"

#include <memory>
#include "loguru/loguru.hpp"

using namespace std;

namespace gradido {
	namespace data {
		using namespace interaction;

		ConfirmedTransaction::ConfirmedTransaction(
			uint64_t id,
			std::shared_ptr<const GradidoTransaction> gradidoTransaction,
			Timestamp confirmedAt,
			const std::string& versionNumber,
			const LedgerAnchor& ledgerAnchor,
			std::vector<AccountBalance> accountBalances,
			BalanceDerivationType balanceDerivationType,
			std::shared_ptr<const ConfirmedTransaction> previousConfirmedTransaction/* = nullptr */
		) : mId(id),
			mGradidoTransaction(gradidoTransaction),
			mConfirmedAt(confirmedAt),
			mVersionNumber(versionNumber),
			mLedgerAnchor(ledgerAnchor),
			mAccountBalances(accountBalances),
			mBalanceDerivationType(balanceDerivationType)
		{
			mRunningHash = calculateRunningHash(previousConfirmedTransaction);
			initalizePubkeyHashes();
		}

		ConfirmedTransaction::ConfirmedTransaction(
			uint64_t id,
			std::shared_ptr<const GradidoTransaction> gradidoTransaction,
			Timestamp confirmedAt,
			const std::string& versionNumber,
			memory::ConstBlockPtr runningHash,
			const LedgerAnchor& ledgerAnchor,
			std::vector<AccountBalance> accountBalances,
			BalanceDerivationType balanceDerivationType
		) : mId(id),
			mGradidoTransaction(gradidoTransaction),
			mConfirmedAt(confirmedAt),
			mVersionNumber(versionNumber),
			mRunningHash(runningHash),
			mLedgerAnchor(ledgerAnchor),
			mAccountBalances(accountBalances),
			mBalanceDerivationType(balanceDerivationType)
		{
			initalizePubkeyHashes();
		}

		memory::ConstBlockPtr ConfirmedTransaction::calculateRunningHash(
			std::shared_ptr<const ConfirmedTransaction> previousConfirmedTransaction/* = nullptr*/
		) const {
			std::string transactionIdString = std::to_string(mId);
			auto confirmedAtString = DataTypeConverter::timePointToString(mConfirmedAt, "%Y-%m-%d %H:%M:%S");
			auto ledgerAnchorString = mLedgerAnchor.toString();
			std::string signatureMapString;
			if (mGradidoTransaction->getSignatureMap().getSignaturePairs().size()) {
				serialize::Context serializeContext(mGradidoTransaction->getSignatureMap());
				signatureMapString = serializeContext.run()->copyAsString();
			}
			auto hash = make_shared<memory::Block>(crypto_generichash_BYTES);

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
			
			crypto_generichash_update(&state, (const unsigned char*)signatureMapString.data(), signatureMapString.size());
			for (auto& accountBalance : mAccountBalances) {
				auto gdd = accountBalance.getBalance().getGradidoCent();
				crypto_generichash_update(&state, (const unsigned char*)&gdd, sizeof(gdd));
			}	
			crypto_generichash_update(&state, (const unsigned char*)&mBalanceDerivationType, sizeof(BalanceDerivationType));
			crypto_generichash_final(&state, hash->data(), hash->size());
			return hash;
		}

		bool ConfirmedTransaction::hasAccountBalance(const memory::Block& publicKey) const
		{
			for (auto& accountBalance : mAccountBalances) {
				if (publicKey.isTheSame(accountBalance.getPublicKey())) {
					return true;
				}
			}
			return false;
		}

		AccountBalance ConfirmedTransaction::getAccountBalance(memory::ConstBlockPtr publicKey, const std::string& communityId) const
		{
			for (auto& accountBalance : mAccountBalances) {
				if (accountBalance.getPublicKey()->isTheSame(publicKey) && accountBalance.getCommunityId().compare(communityId) == 0) {
					return accountBalance;
				}
			}
			return AccountBalance(publicKey, GradidoUnit::zero(), communityId);
		}

		bool ConfirmedTransaction::isInvolved(const memory::Block& publicKey) const
		{
			if (!publicKey.hash().empty()) {
				bool allFalse = true;
				for (auto& hash : mPubkeyHashes) {
					if (hash == publicKey.hash()) {
						allFalse = false;
						break;
					}
				}
				if (allFalse) {
					return false;
				}
			}
			if (isBalanceUpdated(publicKey)) {
				return true;
			}
			return getGradidoTransaction()->isInvolved(publicKey);
		}

		bool ConfirmedTransaction::isBalanceUpdated(const memory::Block& publicKey) const
		{
			for (auto& accountBalance : mAccountBalances) {
				if (accountBalance.getPublicKey()->isTheSame(publicKey)) {
					return true;
				}
			}
			return false;
		}

		std::vector<memory::ConstBlockPtr> ConfirmedTransaction::getInvolvedAddresses() const
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

			if (mVersionNumber != other.mVersionNumber) {
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
			auto involvedAddresses = getInvolvedAddresses();
			mPubkeyHashes.reserve(involvedAddresses.size());
			for (auto& address : involvedAddresses) {
				mPubkeyHashes.emplace_back(address->hash());
			}
		}
	}
}