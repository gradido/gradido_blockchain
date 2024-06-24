#ifndef __GRADIDO_BLOCKCHAIN_V3_3_BLOCKCHAIN_IN_MEMORY_PROVIDER_H
#define __GRADIDO_BLOCKCHAIN_V3_3_BLOCKCHAIN_IN_MEMORY_PROVIDER_H

#include "gradido_blockchain/lib/StringViewCompare.h"
#include "AbstractProvider.h"
#include "InMemory.h"
#include "gradido_blockchain/export.h"

#include <map>
#include <mutex>

namespace gradido {
	namespace v3_3 {
		namespace blockchain {
			class GRADIDOBLOCKCHAIN_EXPORT InMemoryProvider final : public AbstractProvider
			{
			public:
				static std::shared_ptr<InMemoryProvider> getInstance();

				std::shared_ptr<Abstract> findBlockchain(std::string_view communityId);
			protected:
				std::map<std::string, std::shared_ptr<InMemory>, StringViewCompare> mBlockchainsPerGroup;
				std::mutex mWorkMutex;

			private:
				InMemoryProvider();
				~InMemoryProvider();

				/* Explicitly disallow copying. */
				InMemoryProvider(const InMemoryProvider&) = delete;
				InMemoryProvider& operator= (const InMemoryProvider&) = delete;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_V3_3_BLOCKCHAIN_IN_MEMORY_PROVIDER_H