#include "gradido_blockchain/model/protopuf/TransferAmount.h"

namespace model {
	namespace protopuf {
		TransferAmount::TransferAmount(const TransferAmountMessage& data)
			: mRecipientPubkey(std::make_shared<CachedMemoryBlock>(data["pubkey"_f].value())),
			mAmountString(data["amount"_f].value_or("0"))
		{
			if (data["community_id"_f].has_value()) {
				mCommunityId = data["community_id"_f].value();
			}
		}

		TransferAmount::TransferAmount(ConstCachedMemoryBlockPtr recipientPubkey, const StringCachedAlloc& amountString, StringCachedAlloc communityId)
			: mRecipientPubkey(recipientPubkey), mAmountString(amountString), mCommunityId(communityId)
		{
		}
	}
}

