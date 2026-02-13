#include "gradido_blockchain/data/adapter/ledgerAnchor.h"
#include "gradido_blockchain/data/adapter/publicKey.h"
#include "gradido_blockchain/data/adapter/signaturePair.h"
#include "gradido_blockchain/data/compact/PublicKeyIndex.h"
#include "gradido_blockchain/data/GradidoTransaction.h"
#include "gradido_blockchain/interaction/deserialize/Context.h"
#include "gradido_blockchain/interaction/serialize/Context.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/serialization/toJsonString.h"
#include "gradido_protobuf_zig.h"

#include "loguru/loguru.hpp"

#include <memory>
#include <vector>

using serialization::toJsonString;
using std::shared_ptr, std::make_shared;
using std::vector;

namespace gradido {
	namespace data {
		using adapter::toPublicKeyIndex;
		using compact::PublicKeyIndex;
		using namespace interaction;

		shared_ptr<const GradidoTransaction> GradidoTransaction::fromGrdw(const grdw_gradido_transaction* grdw_tx, uint32_t communityIdIndex)
		{
			SignatureMap signatures(grdw_tx->sig_map_count);
			for (size_t i = 0; i < grdw_tx->sig_map_count; ++i) {
				signatures.push(adapter::fromGrdw(&grdw_tx->sig_map[i]));
			}
			auto bodyBytes = make_shared<const Block>(grdw_tx->body_bytes_size, grdw_tx->body_bytes);
			return make_shared<const GradidoTransaction>(signatures, bodyBytes, communityIdIndex, adapter::fromGrdw(grdw_tx->pairing_ledger_anchor));
		}

		void GradidoTransaction::toGrdw(grdu_memory* alloc, grdw_gradido_transaction* grdw_tx, uint32_t communityIdIndex) const
		{
			const auto& sigPairs = mSignatureMap.getSignaturePairs();
			if (sigPairs.size()) {
				grdw_gradido_transaction_reserve_sig_map(alloc, grdw_tx, sigPairs.size());
				if (grdw_tx->sig_map) {
					for (size_t i = 0; i < sigPairs.size(); i++) {
						grdw_tx->sig_map[i] = adapter::toGrdw(sigPairs[i]);
					}
				}
			}
			else {
				grdw_tx->sig_map_count = 0;
			}
			if (mBodyBytes && mBodyBytes->size()) {
				grdw_gradido_transaction_set_body_bytes(alloc, grdw_tx, mBodyBytes->data(), mBodyBytes->size());
			}
			else {
				grdw_tx->body_bytes_size = 0;
			}
			grdw_tx->pairing_ledger_anchor = adapter::toGrdw(alloc, mPairingLedgerAnchor);
		}

		ConstTransactionBodyPtr GradidoTransaction::getTransactionBody() const
		{
			std::scoped_lock _lock(mTransactionBodyMutex);
			if (mTransactionBody) return mTransactionBody;
			if(!mBodyBytes || mBodyBytes->isEmpty()) {
				throw GradidoNullPointerException("body bytes missing", "bodyBytes", __FUNCTION__);
			}

			deserialize::Context c(mBodyBytes, deserialize::Type::TRANSACTION_BODY);
			c.run(mCommunityIdIndex);
			if (!c.isTransactionBody()) {
				LOG_F(ERROR, "Transaction Body:\nxxd -r -ps <<< \"%s\" | protoscope\ncannot be deserialized", mBodyBytes->convertToHex().c_str());
				throw GradidoNullPointerException("cannot deserialize from body bytes", "TransactionBody", __FUNCTION__);
			}
			mTransactionBody = c.getTransactionBody();
			return mTransactionBody;
		}
		bool GradidoTransaction::isPairing(const GradidoTransaction& other) const
		{
			return getTransactionBody()->isPairing(*other.getTransactionBody());
		}

		bool GradidoTransaction::isInvolved(const memory::Block& publicKey) const
		{
			for (auto& signPair : mSignatureMap.getSignaturePairs()) {
				if (signPair.getPublicKey()->isTheSame(publicKey)) {
					return true;
				}
			}
			return getTransactionBody()->isInvolved(publicKey);
		}

		bool GradidoTransaction::isInvolved(const compact::PublicKeyIndex publicKeyIndex) const
		{
			for (auto& signPair : mSignatureMap.getSignaturePairs()) {
				if (toPublicKeyIndex(signPair.getPublicKey(), mCommunityIdIndex) == publicKeyIndex) {
					return true;
				}
			}
			return getTransactionBody()->isInvolved(publicKeyIndex);
		}

		std::vector<memory::ConstBlockPtr> GradidoTransaction::getInvolvedAddresses() const
		{
			auto involvedAddresses = getTransactionBody()->getInvolvedAddresses();
			for (auto& signPair : mSignatureMap.getSignaturePairs()) {
				bool found = false;
				for (auto& involvedAddress : involvedAddresses) {
					if (involvedAddress->isTheSame(signPair.getPublicKey())) {
						found = true;
						break;
					}
				}
				if (!found) {
					involvedAddresses.push_back(signPair.getPublicKey());
				}
			}
			return involvedAddresses;
		}

		vector<PublicKeyIndex> GradidoTransaction::getInvolvedAddressIndices() const
		{
			auto involvedAddressIndices = getTransactionBody()->getInvolvedAddressIndices();
			for (auto& signPair : mSignatureMap.getSignaturePairs()) {
				bool found = false;
				auto sigPairPublicKeyIndex = toPublicKeyIndex(signPair.getPublicKey(), mCommunityIdIndex);
				for (auto& involvedAddress : involvedAddressIndices) {
					if (involvedAddress == sigPairPublicKeyIndex) {
						found = true;
						break;
					}
				}
				if (!found) {
					involvedAddressIndices.emplace_back(sigPairPublicKeyIndex);
				}
			}
			return involvedAddressIndices;
		}

		memory::ConstBlockPtr GradidoTransaction::getSerializedTransaction() const
		{
			std::lock_guard _lock(mSerializedTransactionMutex);
			if (mSerializedTransaction) {
				return mSerializedTransaction;
			}
			mSerializedTransaction = serialize::Context(*this).run();
			return mSerializedTransaction;
		}

		memory::ConstBlockPtr GradidoTransaction::getFingerprint() const {
			if (mSignatureMap.getSignaturePairs().size()) {
				return mSignatureMap.getSignaturePairs()[0].getSignature();
			}
			if (!mBodyBytes) throw InvalidGradidoTransaction("missing body bytes", getSerializedTransaction());
			return std::make_shared<memory::Block>(mBodyBytes->calculateHash());
		}
		bool GradidoTransaction::isTheSame(const GradidoTransaction& other) const
		{
			return mSignatureMap.isTheSame(other.mSignatureMap) && mBodyBytes->isTheSame(other.mBodyBytes);
		}
	}
}