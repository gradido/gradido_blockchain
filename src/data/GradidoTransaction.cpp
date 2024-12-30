#include "gradido_blockchain/data/GradidoTransaction.h"
#include "gradido_blockchain/interaction/deserialize/Context.h"
#include "gradido_blockchain/interaction/serialize/Context.h"

namespace gradido {
	namespace data {
		using namespace interaction;

		ConstTransactionBodyPtr GradidoTransaction::getTransactionBody() const
		{
			std::scoped_lock _lock(mTransactionBodyMutex);
			if (mTransactionBody) return mTransactionBody;
			if(!mBodyBytes || mBodyBytes->isEmpty()) {
				throw GradidoNullPointerException("body bytes missing", "bodyBytes", __FUNCTION__);
			}

			deserialize::Context c(mBodyBytes, deserialize::Type::TRANSACTION_BODY);
			c.run();
			if (!c.isTransactionBody()) {
				throw GradidoNullPointerException("cannot deserialize from body bytes", "TransactionBody", __FUNCTION__);
			}
			mTransactionBody = c.getTransactionBody();
			return mTransactionBody;
		}
		bool GradidoTransaction::isPairing(const GradidoTransaction& other) const
		{
			auto& sigPairs = mSignatureMap.getSignaturePairs();
			auto& otherSigPairs = other.getSignatureMap().getSignaturePairs();

			// compare signature pairs, should be all the same 
			if (sigPairs.size() != otherSigPairs.size() ||
				!std::equal(sigPairs.begin(), sigPairs.end(), otherSigPairs.begin())
				) {
				return false;
			}
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
	}
}