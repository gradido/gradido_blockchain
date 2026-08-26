#ifndef __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_IN_MEMORY_PROVIDER_H
#define __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_IN_MEMORY_PROVIDER_H

#include "AbstractProvider.h"
#include "InMemory.h"
#include "gradido_blockchain/export.h"

#include <unordered_map>
#include <string>
#include <shared_mutex>
#include <optional>

namespace gradido {
	namespace blockchain {
		class GRADIDOBLOCKCHAIN_EXPORT InMemoryProvider final : public AbstractProvider
		{
		public:
			static InMemoryProvider* getInstance();

			std::shared_ptr<Abstract> findBlockchain(uint32_t communityIdIndex) override;
			std::shared_ptr<Abstract> findBlockchain(const std::string& communityId) override;
			void clear();
		protected:
			std::unordered_map<uint32_t, std::shared_ptr<InMemory>> mBlockchainsPerGroup;
			mutable std::shared_mutex mWorkMutex;

		private:
			InMemoryProvider();
			~InMemoryProvider();
			std::shared_ptr<Abstract> addBlockchain(const std::string& communityId, std::optional<uint32_t> communityIdIndex);

			/* Explicitly disallow copying. */
			InMemoryProvider(const InMemoryProvider&) = delete;
			InMemoryProvider& operator= (const InMemoryProvider&) = delete;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_BLOCKCHAIN_IN_MEMORY_PROVIDER_H