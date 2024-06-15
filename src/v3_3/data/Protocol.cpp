#include "gradido_blockchain/v3_3/data/Protocol.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"
#include "gradido_blockchain/v3_3/interaction/serialize/Context.h"
#include "gradido_blockchain/v3_3/interaction/deserialize/Context.h"

#include "sodium.h"

#include <algorithm>
#include <chrono>

using namespace std::chrono;

namespace gradido {
	namespace v3_3 {
		using namespace interaction;
		namespace data {

			const char* GRADIDO_PROTOCOL_VERSION = "3.3";

			void SignatureMap::push(const SignaturePair& signaturePair)
			{
				// check if pubkey already exist
				for (auto& signaturePairIt : signaturePairs) {
					if (signaturePair.pubkey->isTheSame(signaturePairIt.pubkey)) {
						throw GradidoAlreadyExist("public key already exist in signature map of gradido transaction");
					}
				}
				signaturePairs.push_back(signaturePair);
			}

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

			ConstTransactionBodyPtr GradidoTransaction::getTransactionBody() const
			{
				if (mTransactionBody) return mTransactionBody;
				deserialize::Context c(bodyBytes, deserialize::Type::TRANSACTION_BODY);
				c.run();
				if (!c.isTransactionBody()) {
					throw GradidoNullPointerException("cannot deserialize from body bytes", "TransactionBody", __FUNCTION__);
				}
				return c.getTransactionBody();
			}
			ConstTransactionBodyPtr GradidoTransaction::getTransactionBody()
			{
				std::scoped_lock _lock(mTransactionBodyMutex);
				mTransactionBody = static_cast<const GradidoTransaction>(*this).getTransactionBody();
				return mTransactionBody;
			}
			bool GradidoTransaction::isPairing(const GradidoTransaction& other) const
			{
				auto& sigPairs = signatureMap.signaturePairs;
				auto& otherSigPairs = signatureMap.signaturePairs;

				// compare signature pairs, should be all the same 
				if (sigPairs.size() != otherSigPairs.size() ||
					!std::equal(sigPairs.begin(), sigPairs.end(), otherSigPairs.begin())
					) {
					return false;
				}
				return getTransactionBody()->isPairing(*other.getTransactionBody());
			}

			memory::ConstBlockPtr GradidoTransaction::getSerializedTransaction()
			{
				std::scoped_lock _lock(mSerializedTransactionMutex);
				mSerializedTransaction = static_cast<const GradidoTransaction>(*this).getSerializedTransaction();
				return mSerializedTransaction;
			}

			memory::ConstBlockPtr GradidoTransaction::getSerializedTransaction() const
			{
				return interaction::serialize::Context(*this).run();
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

			data::TransactionType TransactionBody::getTransactionType() const
			{
				if (isTransfer()) return data::TransactionType::TRANSFER;
				else if (isCreation()) return data::TransactionType::CREATION;
				else if (isCommunityFriendsUpdate()) return data::TransactionType::COMMUNITY_FRIENDS_UPDATE;
				else if (isRegisterAddress()) return data::TransactionType::REGISTER_ADDRESS;
				else if (isDeferredTransfer()) return data::TransactionType::DEFERRED_TRANSFER;
				else if (isCommunityRoot()) return data::TransactionType::COMMUNITY_ROOT;
				return data::TransactionType::NONE;
			}

			bool TransactionBody::isPairing(const TransactionBody& other) const
			{
				// memo, type and createdAt must be the same, otherGroup must be different
				if (memo != other.memo ||
					createdAt != other.createdAt ||
					otherGroup == other.otherGroup
					) {
					return false;
				}
				if (isCommunityFriendsUpdate() && other.isCommunityFriendsUpdate()) {
					return *communityFriendsUpdate == *other.communityFriendsUpdate;
				}
				if (isRegisterAddress() && other.isRegisterAddress()) {
					return *registerAddress == *other.registerAddress;
				}
				if (isTransfer() && other.isTransfer()) {
					return *transfer == *other.transfer;
				}
				return false;
			}
		}
	}
}