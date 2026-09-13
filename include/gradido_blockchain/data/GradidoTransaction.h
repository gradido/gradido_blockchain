#ifndef __GRADIDO_BLOCKCHAIN_DATA_GRADIDO_TRANSACTION_H
#define __GRADIDO_BLOCKCHAIN_DATA_GRADIDO_TRANSACTION_H

#include "TransactionBody.h"
#include "SignatureMap.h"
#include "LedgerAnchor.h"

struct grdw_gradido_transaction;
struct arnm;

namespace gradido {
	class GradidoTransactionBuilder;
	namespace data {
		class GRADIDOBLOCKCHAIN_EXPORT GradidoTransaction
		{
			friend GradidoTransactionBuilder;
		public:
			GradidoTransaction(): mCommunityIdIndex(0) {}
			GradidoTransaction(
				const SignatureMap& signatureMap,
				memory::ConstBlockPtr bodyBytes,
				uint32_t communityIdIndex,
				const LedgerAnchor& pairingLedgerAnchor = LedgerAnchor()
			) : mSignatureMap(signatureMap), mBodyBytes(bodyBytes), mCommunityIdIndex(communityIdIndex), mPairingLedgerAnchor(pairingLedgerAnchor) {
				int zahl = 1;
			}

			// copy constructor
			GradidoTransaction(const GradidoTransaction& other)
				: GradidoTransaction(other.mSignatureMap, other.mBodyBytes, other.mCommunityIdIndex, other.mPairingLedgerAnchor) {
				mTransactionBody = other.getTransactionBody();
			}

			~GradidoTransaction() {}

			static std::shared_ptr<const GradidoTransaction> fromGrdw(const grdw_gradido_transaction* grdw_tx, uint32_t communityIdIndex);
			void toGrdw(arnm* alloc, grdw_gradido_transaction* grdw_tx, uint32_t communityIdIndex) const;

			//! will deserialize just once and cache the result
			ConstTransactionBodyPtr getTransactionBody() const;
			//! will deserialize transactionBody if not cached
			bool isPairing(const GradidoTransaction& other) const;
			[[deprecated("Replaced by isInvolved with compact::PublicKeyIndex")]]
			bool isInvolved(const memory::Block& publicKey) const;
			bool isInvolved(const compact::PublicKeyIndex publicKeyIndex) const;
			[[deprecated("Replaced by getInvolvedAddressIndices")]]
			std::vector<memory::ConstBlockPtr> getInvolvedAddresses() const;
			std::vector<compact::PublicKeyIndex> getInvolvedAddressIndices() const;
			//! will serialize just once and cache the result
			memory::ConstBlockPtr getSerializedTransaction() const;
			//! return hash for clearly identify the transaction
			//! normally the first signature, but if not exist return hash from bodyBytes
			memory::ConstBlockPtr getFingerprint() const;
			bool isTheSame(const GradidoTransaction& other) const;

			inline const SignatureMap& getSignatureMap() const { return mSignatureMap; }
			inline SignatureMap& getSignatureMap() { return mSignatureMap; }
			inline memory::ConstBlockPtr getBodyBytes() const { return mBodyBytes; }
			inline uint32_t getCommunityIdIndex() const { return mCommunityIdIndex; }
			inline const LedgerAnchor& getPairingLedgerAnchor() const { return mPairingLedgerAnchor; }

		protected:
			SignatureMap			mSignatureMap;
			memory::ConstBlockPtr	mBodyBytes;
			uint32_t					mCommunityIdIndex;
			LedgerAnchor			mPairingLedgerAnchor;

			mutable ConstTransactionBodyPtr mTransactionBody;
			mutable std::mutex mTransactionBodyMutex;
			mutable memory::ConstBlockPtr mSerializedTransaction;
			mutable std::mutex mSerializedTransactionMutex;
		};

		typedef std::shared_ptr<const GradidoTransaction> ConstGradidoTransactionPtr;
	}
}

#endif //__GRADIDO_BLOCKCHAIN_DATA_GRADIDO_TRANSACTION_H