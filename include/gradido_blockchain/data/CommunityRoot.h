#ifndef __GRADIDO_BLOCKCHAIN_DATA_COMMUNITY_ROOT_H
#define __GRADIDO_BLOCKCHAIN_DATA_COMMUNITY_ROOT_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/memory/Block.h"

namespace gradido {
	namespace data {
		class GRADIDOBLOCKCHAIN_EXPORT CommunityRoot
		{
		public:
			// throw InvalidSizeException
			CommunityRoot(
				memory::ConstBlockPtr pubkeyPtr,
				memory::ConstBlockPtr gmwPubkeyPtr,
				memory::ConstBlockPtr aufPubkeyPtr
			);

			inline std::vector<memory::ConstBlockPtr> getInvolvedAddresses() const { return { mPubkey, mGmwPubkey, mAufPubkey }; }
			bool isInvolved(const memory::Block& publicKey) const;

			inline memory::ConstBlockPtr getPubkey() const { return mPubkey; }
			inline memory::ConstBlockPtr getGmwPubkey() const { return mGmwPubkey; }
			inline memory::ConstBlockPtr getAufPubkey() const { return mAufPubkey; }

		protected:
			memory::ConstBlockPtr mPubkey;
			memory::ConstBlockPtr mGmwPubkey;
			memory::ConstBlockPtr mAufPubkey;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_DATA_COMMUNITY_ROOT_H