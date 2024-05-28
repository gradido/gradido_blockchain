#ifndef __GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_TRANSFER_AMOUNT_H
#define __GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_TRANSFER_AMOUNT_H

#include <bit>
#include "protopuf/message.h"
#include "VectorCacheAllocator.h"
#include "gradido_blockchain/MemoryManager.h"
#include "gradido_blockchain/lib/Decimal.h"
#include "gradido_blockchain/export.h"

using namespace pp;

namespace model {
	namespace protopuf {
        using TransferAmountMessage = message<
            bytes_field<"pubkey", 1, singular, std::vector<ByteVectorCachedAlloc, VectorCacheAllocator<ByteVectorCachedAlloc>>>,
            string_field<"amount", 2, singular, std::vector<StringCachedAlloc>>,
            string_field<"community_id", 3, singular, std::vector<StringCachedAlloc>>
        >;

        class GRADIDOBLOCKCHAIN_EXPORT TransferAmount
        {
        public:
            TransferAmount(const TransferAmountMessage& data);
            TransferAmount(ConstCachedMemoryBlockPtr recipientPubkey, const StringCachedAlloc& amountString, StringCachedAlloc communityId);
            
            ~TransferAmount() {}

            inline const ConstCachedMemoryBlockPtr getPubkey() const { return mRecipientPubkey; }
            inline const StringCachedAlloc getAmountString() const { return mAmountString; }
            // inline const Decimal getAmount() const { return mAmountString; }
            inline const StringCachedAlloc getCommunityId() const { return mCommunityId; }
        protected:
            ConstCachedMemoryBlockPtr mRecipientPubkey;
            StringCachedAlloc mAmountString;
            StringCachedAlloc mCommunityId;

        };
	}
}

#endif //__GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_TRANSFER_AMOUNT_H