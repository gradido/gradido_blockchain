#ifndef __GRADIDO_BLOCKCHAIN_DATA_GRADIDO_TRANSACTION_H
#define __GRADIDO_BLOCKCHAIN_DATA_GRADIDO_TRANSACTION_H

#include "TransactionBody.h"
#include "SignatureMap.h"

namespace gradido {
	class GradidoTransactionBuilder;
	namespace data {
		class GRADIDOBLOCKCHAIN_EXPORT GradidoTransaction
		{
			friend GradidoTransactionBuilder;
		public:
			GradidoTransaction() {}
			GradidoTransaction(
				const SignatureMap& signatureMap,
				memory::ConstBlockPtr bodyBytes,
				memory::ConstBlockPtr paringMessageId = nullptr
			) : mSignatureMap(signatureMap), mBodyBytes(bodyBytes), mParingMessageId(paringMessageId) {}

			// copy constructor
			GradidoTransaction(const GradidoTransaction& other)
				: GradidoTransaction(other.mSignatureMap, other.mBodyBytes, other.mParingMessageId) {}

			~GradidoTransaction() {}

			//! will deserialize just once and cache the result
			ConstTransactionBodyPtr getTransactionBody() const;
			//! will deserialize transactionBody if not cached
			bool isPairing(const GradidoTransaction& other) const;
			bool isInvolved(memory::ConstBlockPtr publicKey) const;
			std::vector<memory::ConstBlockPtr> getInvolvedAddresses() const;
			//! will serialize just once and cache the result
			memory::ConstBlockPtr getSerializedTransaction() const;
			//! return hash for clearly identify the transaction
			//! normally the first signature, but if not exist return hash from bodyBytes
			memory::ConstBlockPtr getFingerprint() const;

			inline const SignatureMap& getSignatureMap() const { return mSignatureMap; }
			inline SignatureMap& getSignatureMap() { return mSignatureMap; }
			inline memory::ConstBlockPtr getBodyBytes() const { return mBodyBytes; }
			inline memory::ConstBlockPtr getParingMessageId() const { return mParingMessageId; }

		protected:
			SignatureMap			mSignatureMap;
			memory::ConstBlockPtr	mBodyBytes;
			memory::ConstBlockPtr	mParingMessageId;

			mutable ConstTransactionBodyPtr mTransactionBody;
			mutable std::mutex mTransactionBodyMutex;
			mutable memory::ConstBlockPtr mSerializedTransaction;
			mutable std::mutex mSerializedTransactionMutex;
		};

		typedef std::shared_ptr<const GradidoTransaction> ConstGradidoTransactionPtr;
	}
}

#endif //__GRADIDO_BLOCKCHAIN_DATA_GRADIDO_TRANSACTION_H