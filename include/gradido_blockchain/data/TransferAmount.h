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
				const std::string& communityId = ""
			);
			~TransferAmount() {}

			inline bool operator==(const TransferAmount& other) const {
				return mPublicKey && mPublicKey->isTheSame(other.mPublicKey) && mAmount == other.mAmount && mCommunityId == other.mCommunityId;
			}
			inline memory::ConstBlockPtr getPublicKey() const { return mPublicKey; }
			inline GradidoUnit getAmount() const { return mAmount; }
			inline const std::string& getCommunityId() const { return mCommunityId; }

			inline void updateCoinColor(const std::string& communityId) { mCommunityId = communityId; }

		protected:
			memory::ConstBlockPtr mPublicKey;
			GradidoUnit mAmount;
			std::string mCommunityId;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_DATA_TRANSFER_AMOUNT_H