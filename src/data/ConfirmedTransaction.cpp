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
			memory::ConstBlockPtr runningHash,
			memory::ConstBlockPtr messageId,
			const std::string& accountBalanceString
		) : id(id),
			gradidoTransaction(std::move(gradidoTransaction)),
			confirmedAt(confirmedAt),
			versionNumber(versionNumber),
			runningHash(runningHash),
			messageId(messageId),
			accountBalance(accountBalanceString) 
		{
		}


		memory::Block ConfirmedTransaction::calculateRunningHash(std::shared_ptr<const ConfirmedTransaction> previousConfirmedTransaction/* = nullptr*/) const
		{
			std::string transactionIdString = std::to_string(id);
			auto confirmedAtString = DataTypeConverter::timePointToString(confirmedAt, "%Y-%m-%d %H:%M:%S");
			std::string signatureMapString;
			if (gradidoTransaction->getSignatureMap().getSignaturePairs().size()) {
				serialize::Context serializeContext(gradidoTransaction->getSignatureMap());
				signatureMapString = serializeContext.run()->copyAsString();
			}
			std::string accountBalanceString = accountBalance.toString();

			memory::Block hash(crypto_generichash_BYTES);

			// Sodium use for the generic hash function BLAKE2b today (11.11.2019), maybe change in the future
			crypto_generichash_state state;
			crypto_generichash_init(&state, nullptr, 0, crypto_generichash_BYTES);
			if (previousConfirmedTransaction && previousConfirmedTransaction->runningHash) {
				auto prevHashHex = previousConfirmedTransaction->runningHash->convertToHex();
				crypto_generichash_update(&state, (const unsigned char*)prevHashHex.data(), prevHashHex.size());
			}
			crypto_generichash_update(&state, (const unsigned char*)transactionIdString.data(), transactionIdString.size());
			//printf("transaction id string: %s\n", transactionIdString.data());
			crypto_generichash_update(&state, (const unsigned char*)confirmedAtString.data(), confirmedAtString.size());
			//printf("received: %s\n", receivedString.data());
			crypto_generichash_update(&state, (const unsigned char*)signatureMapString.data(), signatureMapString.size());
			//printf("signature map serialized: %s\n", convertBinToHex(signatureMapString).data());
			crypto_generichash_update(&state, (const unsigned char*)accountBalanceString.data(), accountBalanceString.size());
			crypto_generichash_final(&state, hash, hash.size());
			return hash;
		}
	}
}