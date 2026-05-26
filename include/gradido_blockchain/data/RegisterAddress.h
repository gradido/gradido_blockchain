#ifndef __GRADIDO_BLOCKCHAIN_DATA_REGISTER_ADDRESS_H
#define __GRADIDO_BLOCKCHAIN_DATA_REGISTER_ADDRESS_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain_core/types/address.h"

namespace gradido {
	namespace data {
		class GRADIDOBLOCKCHAIN_EXPORT RegisterAddress
		{
		public:
			RegisterAddress(
				grdt_address _addressType,
				uint32_t _derivationIndex,
				memory::ConstBlockPtr userPubkeyPtr,
				memory::ConstBlockPtr nameHashPtr,
				memory::ConstBlockPtr accountPubkeyPtr
			);

			~RegisterAddress() {}

			inline bool operator==(const RegisterAddress& other) const {
				return
					mUserPubkey && mUserPubkey->isTheSame(other.mUserPubkey) &&
					mgrdt_address == other.mgrdt_address &&
					mNameHash && mNameHash->isTheSame(other.mNameHash) &&
					mAccountPubkey && mAccountPubkey->isTheSame(other.mAccountPubkey) &&
					mDerivationIndex == other.mDerivationIndex
					;
			}

			std::vector<memory::ConstBlockPtr> getInvolvedAddresses() const;
			bool isInvolved(const memory::Block& publicKey) const;

			inline memory::ConstBlockPtr getUserPublicKey() const { return mUserPubkey; }
			inline grdt_address getAddressType() const { return mgrdt_address; }
			inline memory::ConstBlockPtr getNameHash() const { return mNameHash; }
			inline memory::ConstBlockPtr getAccountPublicKey() const { return mAccountPubkey; }
			inline uint32_t getDerivationIndex() const { return mDerivationIndex; }

		protected:
			memory::ConstBlockPtr	mUserPubkey;
			grdt_address				mgrdt_address;
			memory::ConstBlockPtr	mNameHash;
			memory::ConstBlockPtr   mAccountPubkey;
			uint32_t				mDerivationIndex;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_DATA_REGISTER_ADDRESS_H