#include "gradido_blockchain/data/RegisterAddress.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"

namespace gradido {
	namespace data {
		RegisterAddress::RegisterAddress(
			grdt_address _addressType,
			uint32_t _derivationIndex,
			memory::ConstBlockPtr userPubkeyPtr,
			memory::ConstBlockPtr nameHashPtr,
			memory::ConstBlockPtr accountPubkeyPtr
		) : mUserPubkey(userPubkeyPtr), mgrdt_address(_addressType), mNameHash(nameHashPtr),
			mAccountPubkey(accountPubkeyPtr), mDerivationIndex(_derivationIndex) 
		{
			KeyPairEd25519::validatePublicKey(userPubkeyPtr);
			KeyPairEd25519::validatePublicKey(accountPubkeyPtr);
			if (accountPubkeyPtr->isTheSame(userPubkeyPtr)) {
				throw GradidoNodeInvalidDataException("accountPubkey and userPubkey are the same");
			}
			if (!nameHashPtr) {
				throw GradidoNullPointerException("nameHash cannot be a nullptr", "memory::Block", __FUNCTION__);
			}
			if (nameHashPtr->isEmpty()) {
				throw GradidoNodeInvalidDataException("nameHash cannot be empty");
			}
			if (crypto_generichash_BYTES != nameHashPtr->size()) {
				throw Ed25519InvalidKeyException("invalid key size for nameHash", *nameHashPtr, crypto_generichash_BYTES);
			}
		}

		std::vector<memory::ConstBlockPtr> RegisterAddress::getInvolvedAddresses() const
		{
			std::vector<memory::ConstBlockPtr> result;
			result.reserve(2);
			if (mUserPubkey) {
				result.push_back(mUserPubkey);
			}
			if (mAccountPubkey) {
				result.push_back(mAccountPubkey);
			}
			return result;
		}

		bool RegisterAddress::isInvolved(const memory::Block& publicKey) const
		{
			if (publicKey.isTheSame(mUserPubkey) || publicKey.isTheSame(mAccountPubkey)) {
				return true;
			}
			return false;
		}
	}
}