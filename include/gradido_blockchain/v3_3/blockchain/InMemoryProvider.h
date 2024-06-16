#ifndef __GRADIDO_BLOCKCHAIN_V3_3_IN_MEMORY_BLOCKCHAIN_PROVIDER_H
#define __GRADIDO_BLOCKCHAIN_V3_3_IN_MEMORY_BLOCKCHAIN_PROVIDER_H

#include "AbstractBlockchainProvider.h"
#include "InMemoryBlockchain.h"
#include "gradido_blockchain/export.h"

#include <map>

namespace gradido {
	namespace v3_3 {
		class GRADIDOBLOCKCHAIN_EXPORT InMemoryBlockchainProvider final: public AbstractBlockchainProvider
		{
		public:
			static InMemoryBlockchainProvider* getInstance(); 

			std::shared_ptr<AbstractBlockchain> findBlockchain(std::string_view communityId);
		protected:
			std::map<std::string, InMemoryBlockchain> mBlockchainsPerGroup;

		private:
			InMemoryBlockchainProvider();
			~InMemoryBlockchainProvider();

			/* Explicitly disallow copying. */
			InMemoryBlockchainProvider(const InMemoryBlockchainProvider&) = delete;
			InMemoryBlockchainProvider& operator= (const InMemoryBlockchainProvider&) = delete;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_V3_3_IN_MEMORY_BLOCKCHAIN_PROVIDER_H