#include "gradido_blockchain/data/Protocol.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"
#include "gradido_blockchain/interaction/serialize/Context.h"
#include "gradido_blockchain/interaction/deserialize/Context.h"

#include "sodium.h"

#include <algorithm>
#include <chrono>

using namespace std::chrono;

namespace gradido {
	using namespace interaction;
	namespace data {

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

		bool CommunityRoot::isInvolved(memory::ConstBlockPtr publicKey) const
		{
			return 
				publicKey->isTheSame(pubkey) || 
				publicKey->isTheSame(gmwPubkey) || 
				publicKey->isTheSame(gmwPubkey);
		}

		bool GradidoTransfer::isInvolved(memory::ConstBlockPtr publicKey) const
		{
			return publicKey->isTheSame(sender.pubkey) || publicKey->isTheSame(recipient);
		}

		std::vector<memory::ConstBlockPtr> RegisterAddress::getInvolvedAddresses() const
		{
			std::vector<memory::ConstBlockPtr> result;
			result.resize(2);
			if (userPubkey) {
				result.push_back(userPubkey);
			}
			if (accountPubkey) {
				result.push_back(accountPubkey);
			}
			return result;
		}

		bool RegisterAddress::isInvolved(memory::ConstBlockPtr publicKey) const
		{
			if (publicKey->isTheSame(userPubkey) || publicKey->isTheSame(accountPubkey)) {
				return true;
			}
			return false;
		}

		ConstTransactionBodyPtr GradidoTransaction::getTransactionBody() const
		{
			std::scoped_lock _lock(mTransactionBodyMutex);
			if (mTransactionBody) return mTransactionBody;

			deserialize::Context c(bodyBytes, deserialize::Type::TRANSACTION_BODY);
			c.run();
			if (!c.isTransactionBody()) {
				throw GradidoNullPointerException("cannot deserialize from body bytes", "TransactionBody", __FUNCTION__);
			}
			mTransactionBody = c.getTransactionBody();
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

		bool GradidoTransaction::isInvolved(memory::ConstBlockPtr publicKey) const
		{
			for (auto& signPair : signatureMap.signaturePairs) {
				if (signPair.pubkey->isTheSame(publicKey)) {
					return true;
				}
			}
			return getTransactionBody()->isInvolved(publicKey);
		}

		std::vector<memory::ConstBlockPtr> GradidoTransaction::getInvolvedAddresses() const
		{
			auto involvedAddresses = getTransactionBody()->getInvolvedAddresses();
			for (auto& signPair : signatureMap.signaturePairs) {
				bool found = false;
				for (auto& involvedAddress : involvedAddresses) {
					if (involvedAddress->isTheSame(signPair.pubkey)) {
						found = true;
						break;
					}
				}
				if (!found) {
					involvedAddresses.push_back(signPair.pubkey);
				}
			}
			return involvedAddresses;
		}

		memory::ConstBlockPtr GradidoTransaction::getSerializedTransaction() const
		{
			std::lock_guard _lock(mSerializedTransactionMutex);
			if (mSerializedTransaction) {
				return mSerializedTransaction;
			}
			mSerializedTransaction = interaction::serialize::Context(*this).run();
			return mSerializedTransaction;
		}

		memory::ConstBlockPtr GradidoTransaction::getFingerprint() const {
			if (signatureMap.signaturePairs.size()) {
				return signatureMap.signaturePairs[0].signature;
			}
			if (!bodyBytes) throw InvalidGradidoTransaction("missing body bytes", getSerializedTransaction());
			return std::make_shared<memory::Block>(bodyBytes->calculateHash());
		}

		memory::Block ConfirmedTransaction::calculateRunningHash(std::shared_ptr<const ConfirmedTransaction> previousConfirmedTransaction/* = nullptr*/) const
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

		bool TransactionBody::isInvolved(memory::ConstBlockPtr publicKey) const
		{
			if (isCommunityRoot()) return communityRoot->isInvolved(publicKey);
			if (isRegisterAddress()) return registerAddress->isInvolved(publicKey);
			if (isTransfer()) return transfer->isInvolved(publicKey);
			if (isCreation()) return creation->isInvolved(publicKey);
			if (isDeferredTransfer()) return deferredTransfer->isInvolved(publicKey);
			return false;
		}

		const TransferAmount* TransactionBody::getTransferAmount() const
		{
			if (isTransfer()) { return &transfer->sender;}
			else if (isDeferredTransfer()) { return &deferredTransfer->transfer.sender; }
			else if (isCreation()) { &creation->recipient;}
			return nullptr;
		}



		std::vector<memory::ConstBlockPtr> TransactionBody::getInvolvedAddresses() const
		{
			if (isCommunityFriendsUpdate()) return {};
			if (isCommunityRoot()) return communityRoot->getInvolvedAddresses();
			if (isRegisterAddress()) return registerAddress->getInvolvedAddresses();
			if (isTransfer()) return transfer->getInvolvedAddresses();
			if (isCreation()) return creation->getInvolvedAddresses();
			if (isDeferredTransfer()) return deferredTransfer->getInvolvedAddresses();
			return {};
		}
	}
}