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
				const GradidoUnit& amount,
				uint32_t coinCommunityIdIndex
			);
			TransferAmount(
				memory::ConstBlockPtr pubkeyPtr,
				const GradidoUnit& amount,
				const std::string& coinCommunityId
			);
			~TransferAmount() {}

			inline bool operator==(const TransferAmount& other) const {
				return mPublicKey && mPublicKey->isTheSame(other.mPublicKey) && mAmount == other.mAmount && mCoinCommunityIdIndex == other.mCoinCommunityIdIndex;
			}
			inline bool isPairing(const TransferAmount& other) const {
				return mPublicKey && mPublicKey->isTheSame(other.mPublicKey)
					&& mAmount == other.mAmount;
			}
			inline memory::ConstBlockPtr getPublicKey() const { return mPublicKey; }
			inline GradidoUnit getAmount() const { return mAmount; }
			inline uint32_t getCoinCommunityIdIndex() const { return mCoinCommunityIdIndex; }

		protected:
			memory::ConstBlockPtr mPublicKey;
			GradidoUnit mAmount;
			uint32_t mCoinCommunityIdIndex;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_DATA_TRANSFER_AMOUNT_H