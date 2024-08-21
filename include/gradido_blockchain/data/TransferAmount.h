#ifndef __GRADIDO_BLOCKCHAIN_DATA_TRANSFER_AMOUNT_H
#define __GRADIDO_BLOCKCHAIN_DATA_TRANSFER_AMOUNT_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/GradidoUnit.h"

namespace gradido {
	namespace data {
		class GRADIDOBLOCKCHAIN_EXPORT TransferAmount
		{
		public:
			TransferAmount(
				memory::ConstBlockPtr pubkeyPtr,
				const std::string& amountString,
				const std::string& communityId = ""
			) : mPubkey(pubkeyPtr), mAmount(amountString), mCommunityId(communityId) {}
			~TransferAmount() {}

			inline bool operator==(const TransferAmount& other) const {
				return mPubkey && mPubkey->isTheSame(other.mPubkey) && mAmount == other.mAmount && mCommunityId == other.mCommunityId;
			}
			inline memory::ConstBlockPtr getPubkey() const { return mPubkey; }
			inline GradidoUnit getAmount() const { return mAmount; }
			inline const std::string& getCommunityId() const { return mCommunityId; }

		protected:
			memory::ConstBlockPtr mPubkey;
			GradidoUnit mAmount;
			std::string mCommunityId;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_DATA_TRANSFER_AMOUNT_H