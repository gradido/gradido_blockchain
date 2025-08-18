#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"
#include "gradido_blockchain/interaction/serialize/Context.h"

#include <memory>

using namespace std;

namespace gradido {
	namespace data {
		using namespace interaction;

		ConfirmedTransaction::ConfirmedTransaction(
			uint64_t id,
			std::shared_ptr<const GradidoTransaction> gradidoTransaction,
			Timepoint confirmedAt,
			const std::string& versionNumber,
			memory::ConstBlockPtr messageId,
			std::vector<AccountBalance> accountBalances,
			std::shared_ptr<const ConfirmedTransaction> previousConfirmedTransaction/* = nullptr */
		) : mId(id),
			mGradidoTransaction(std::move(gradidoTransaction)),
			mConfirmedAt(confirmedAt),
			mVersionNumber(versionNumber),
			mMessageId(messageId),
			mAccountBalances(accountBalances) 
		{
			mRunningHash = calculateRunningHash(previousConfirmedTransaction);
		}

		ConfirmedTransaction::ConfirmedTransaction(
			uint64_t id,
			std::shared_ptr<const GradidoTransaction> gradidoTransaction,
			Timepoint confirmedAt,
			const std::string& versionNumber,
			memory::ConstBlockPtr runningHash,
			memory::ConstBlockPtr messageId,
			std::vector<AccountBalance> accountBalances
		) : mId(id),
			mGradidoTransaction(std::move(gradidoTransaction)),
			mConfirmedAt(confirmedAt),
			mVersionNumber(versionNumber),
			mRunningHash(runningHash),
			mMessageId(messageId),
			mAccountBalances(accountBalances)
		{

		}

		memory::ConstBlockPtr ConfirmedTransaction::calculateRunningHash(
			std::shared_ptr<const ConfirmedTransaction> previousConfirmedTransaction/* = nullptr*/
		) const {
			std::string transactionIdString = std::to_string(mId);
			auto confirmedAtString = DataTypeConverter::timePointToString(mConfirmedAt, "%Y-%m-%d %H:%M:%S");
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
			
			crypto_generichash_update(&state, (const unsigned char*)signatureMapString.data(), signatureMapString.size());
			for (auto& accountBalance : mAccountBalances) {
				auto gdd = accountBalance.getBalance().getGradidoCent();
				crypto_generichash_update(&state, (const unsigned char*)&gdd, sizeof(gdd));
			}			
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

		AccountBalance ConfirmedTransaction::getAccountBalance(memory::ConstBlockPtr publicKey) const
		{
			for (auto& accountBalance : mAccountBalances) {
				if (accountBalance.getPublicKey()->isTheSame(publicKey)) {
					return accountBalance;
				}
			}
			return AccountBalance(publicKey, GradidoUnit::zero());
		}

		bool ConfirmedTransaction::isInvolved(const memory::Block& publicKey) const
		{
			for (auto& accountBalance: mAccountBalances) {
				if (accountBalance.getPublicKey()->isTheSame(publicKey)) {
					return true;
				}
			}
			return getGradidoTransaction()->isInvolved(publicKey);
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
	}
}