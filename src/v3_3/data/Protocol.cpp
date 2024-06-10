#include "gradido_blockchain/v3_3/data/Protocol.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"
#include "gradido_blockchain/v3_3/interaction/serialize/Context.h"
#include "sodium.h"

#include <chrono>

using namespace std::chrono;

namespace gradido {
	namespace v3_3 {
		using namespace interaction;
		namespace data {

			Timestamp::Timestamp(const Timepoint& date)
				: seconds(0), nanos(0)
			{
				// Convert time_point to duration since epoch
				auto duration = date.time_since_epoch();

				// Convert duration to seconds and nanoseconds
				auto secondsDuration = duration_cast<std::chrono::seconds>(duration);
				seconds = secondsDuration.count();
				nanos = (duration_cast<std::chrono::nanoseconds>(duration) - duration_cast<std::chrono::nanoseconds>(secondsDuration)).count();
			}

			TimestampSeconds::TimestampSeconds(const Timepoint& date)
				: seconds(0)
			{
				// Get the duration since epoch
				auto duration = date.time_since_epoch();

				// Convert the duration to seconds
				seconds = duration_cast<std::chrono::seconds>(duration).count();
			}

			memory::Block ConfirmedTransaction::calculateRunningHash(std::shared_ptr<ConfirmedTransaction> previousConfirmedTransaction/* = nullptr*/)
			{
				std::string transactionIdString = std::to_string(id);
				auto confirmedAtString = DataTypeConverter::timePointToString(confirmedAt, "%Y-%m-%d %H:%M:%S");
				serialize::Context serializeContext(gradidoTransaction->signatureMap);
				std::string signatureMapString = serializeContext.run()->copyAsString();
				std::string accountBalanceString = accountBalance.toString(25);

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
}