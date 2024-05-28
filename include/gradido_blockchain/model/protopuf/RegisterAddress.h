#ifndef __GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_REGISTER_ADDRESS_H
#define __GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_REGISTER_ADDRESS_H

#include <bit>
#include "protopuf/message.h"

#include "VectorCacheAllocator.h"

#include "gradido_blockchain/model/AddressType.h"
#include "gradido_blockchain/export.h"
#include "gradido_blockchain/MemoryManager.h"

using namespace pp;

namespace model {
	namespace protopuf {

		using RegisterAddressMessage = message<
			bytes_field<"user_pubkey", 1, singular, std::vector<ByteVectorCachedAlloc, VectorCacheAllocator<ByteVectorCachedAlloc>>>,
			enum_field<"address_type", 2, AddressType>,
			bytes_field<"name_hash", 3, singular, std::vector<ByteVectorCachedAlloc, VectorCacheAllocator<ByteVectorCachedAlloc>>>,
			bytes_field<"account_pubkey", 4, singular, std::vector<ByteVectorCachedAlloc, VectorCacheAllocator<ByteVectorCachedAlloc>>>,
			uint32_field<"derivation_index", 5>
		>;

		class GRADIDOBLOCKCHAIN_EXPORT RegisterAddress
		{
		public:
			RegisterAddress(const RegisterAddressMessage& data);
			RegisterAddress(
				ConstCachedMemoryBlockPtr userPubkey,
				AddressType addressType,
				uint32_t derivationIndex = 1,
				ConstCachedMemoryBlockPtr nameHash = nullptr,
				ConstCachedMemoryBlockPtr accountPubkey = nullptr
			);

			~RegisterAddress() {}

			inline ConstCachedMemoryBlockPtr getUserPubkey() const { return mUserPubkey; }
			inline AddressType getAddressType() const { return mAddressType; }
			inline ConstCachedMemoryBlockPtr getNameHash() const { return mNameHash; }
			inline ConstCachedMemoryBlockPtr getAccountPubkey() const { return mAccountPubkey; }
			inline uint32_t getDerivationIndex() const { return mDerivationIndex; }

		protected:
			ConstCachedMemoryBlockPtr	mUserPubkey;
			AddressType					mAddressType;
			ConstCachedMemoryBlockPtr	mNameHash;
			ConstCachedMemoryBlockPtr	mAccountPubkey;
			uint32_t					mDerivationIndex;			
		};
	}
}

#endif // __GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_REGISTER_ADDRESS_H