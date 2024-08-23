#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"
#include "gradido_blockchain/interaction/serialize/Context.h"

namespace gradido {
	namespace data {
		using namespace interaction;

		ConfirmedTransaction::ConfirmedTransaction(
			uint64_t id,
			std::shared_ptr<const GradidoTransaction> gradidoTransaction,
			Timepoint confirmedAt,
			const std::string& versionNumber,
			memory::ConstBlockPtr messageId,
			const std::string& accountBalanceString,
			std::shared_ptr<const ConfirmedTransaction> previousConfirmedTransaction/* = nullptr */
		) : mId(id),
			mGradidoTransaction(std::move(gradidoTransaction)),
			mConfirmedAt(confirmedAt),
			mVersionNumber(versionNumber),
			mMessageId(messageId),
			mAccountBalance(accountBalanceString) 
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
			const std::string& accountBalanceString
		) : mId(id),
			mGradidoTransaction(std::move(gradidoTransaction)),
			mConfirmedAt(confirmedAt),
			mVersionNumber(versionNumber),
			mRunningHash(runningHash),
			mMessageId(messageId),
			mAccountBalance(accountBalanceString)
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
			std::string accountBalanceString = mAccountBalance.toString();

			auto hash = make_shared<memory::Block>(crypto_generichash_BYTES);

			// Sodium use for the generic hash function BLAKE2b today (11.11.2019), maybe change in the future
			crypto_generichash_state state;
			crypto_generichash_init(&state, nullptr, 0, crypto_generichash_BYTES);
			if (previousConfirmedTransaction && previousConfirmedTransaction->mRunningHash) {
				auto prevHashHex = previousConfirmedTransaction->mRunningHash->convertToHex();
				crypto_generichash_update(&state, (const unsigned char*)prevHashHex.data(), prevHashHex.size());
			}
			crypto_generichash_update(&state, (const unsigned char*)transactionIdString.data(), transactionIdString.size());
			//printf("transaction id string: %s\n", transactionIdString.data());
			crypto_generichash_update(&state, (const unsigned char*)confirmedAtString.data(), confirmedAtString.size());
			//printf("received: %s\n", receivedString.data());
			crypto_generichash_update(&state, (const unsigned char*)signatureMapString.data(), signatureMapString.size());
			//printf("signature map serialized: %s\n", convertBinToHex(signatureMapString).data());
			crypto_generichash_update(&state, (const unsigned char*)accountBalanceString.data(), accountBalanceString.size());
			crypto_generichash_final(&state, hash->data(), hash->size());
			return hash;
		}
	}
}