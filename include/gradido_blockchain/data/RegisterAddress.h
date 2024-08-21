#ifndef __GRADIDO_BLOCKCHAIN_DATA_REGISTER_ADDRESS_H
#define __GRADIDO_BLOCKCHAIN_DATA_REGISTER_ADDRESS_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/memory/Block.h"
#include "AddressType.h"

namespace gradido {
	namespace data {
		class GRADIDOBLOCKCHAIN_EXPORT RegisterAddress
		{
		public:
			RegisterAddress(
				AddressType _addressType,
				uint32_t _derivationIndex = 1,
				memory::ConstBlockPtr userPubkeyPtr = nullptr,
				memory::ConstBlockPtr nameHashPtr = nullptr,
				memory::ConstBlockPtr accountPubkeyPtr = nullptr
			) : mUserPubkey(userPubkeyPtr), mAddressType(_addressType), mNameHash(nameHashPtr),
				mAccountPubkey(accountPubkeyPtr), mDerivationIndex(_derivationIndex) {}

			~RegisterAddress() {}

			inline bool operator==(const RegisterAddress& other) const {
				return
					mUserPubkey && mUserPubkey->isTheSame(other.mUserPubkey) &&
					mAddressType == other.mAddressType &&
					mNameHash && mNameHash->isTheSame(other.mNameHash) &&
					mAccountPubkey && mAccountPubkey->isTheSame(other.mAccountPubkey) &&
					mDerivationIndex == other.mDerivationIndex
					;
			}

			std::vector<memory::ConstBlockPtr> getInvolvedAddresses() const;
			bool isInvolved(memory::ConstBlockPtr publicKey) const;

			inline memory::ConstBlockPtr getUserPublicKey() const { return mUserPubkey; }
			inline AddressType getAddressType() const { return mAddressType; }
			inline memory::ConstBlockPtr getNameHash() const { return mNameHash; }
			inline memory::ConstBlockPtr getAccountPublicKey() const { return mAccountPubkey; }
			inline uint32_t getDerivationIndex() const { return mDerivationIndex; }

		protected:
			memory::ConstBlockPtr	mUserPubkey;
			AddressType				mAddressType;
			memory::ConstBlockPtr	mNameHash;
			memory::ConstBlockPtr   mAccountPubkey;
			uint32_t				mDerivationIndex;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_DATA_REGISTER_ADDRESS_H