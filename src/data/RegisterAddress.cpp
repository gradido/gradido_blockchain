#include "gradido_blockchain/data/RegisterAddress.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"

namespace gradido {
	namespace data {
		RegisterAddress::RegisterAddress(
			AddressType _addressType,
			uint32_t _derivationIndex /* = 1*/,
			memory::ConstBlockPtr userPubkeyPtr /* = nullptr*/,
			memory::ConstBlockPtr nameHashPtr /* = nullptr*/,
			memory::ConstBlockPtr accountPubkeyPtr /* = nullptr*/
		) : mUserPubkey(userPubkeyPtr), mAddressType(_addressType), mNameHash(nameHashPtr),
			mAccountPubkey(accountPubkeyPtr), mDerivationIndex(_derivationIndex) 
		{
			if (userPubkeyPtr) {
				KeyPairEd25519::validatePublicKey(userPubkeyPtr);
			}
			if (accountPubkeyPtr) {
				KeyPairEd25519::validatePublicKey(accountPubkeyPtr);
			}
			if (accountPubkeyPtr && userPubkeyPtr && accountPubkeyPtr->isTheSame(userPubkeyPtr)) {
				throw GradidoNodeInvalidDataException("accountPubkey and userPubkey are the same");
			}
			if (!accountPubkeyPtr && !userPubkeyPtr) {
				throw GradidoNodeInvalidDataException("accountPubkey and userPubkey are both nullptr, at least one is needed");
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

		bool RegisterAddress::isInvolved(memory::ConstBlockPtr publicKey) const
		{
			if (publicKey->isTheSame(mUserPubkey) || publicKey->isTheSame(mAccountPubkey)) {
				return true;
			}
			return false;
		}
	}
}