#include "gradido_blockchain/v3_3/blockchain/InMemoryProvider.h"
#include "gradido_blockchain/v3_3/blockchain/Exceptions.h"

namespace gradido {
	namespace v3_3 {
		namespace blockchain {
			InMemoryProvider::InMemoryProvider()
			{

			}

			InMemoryProvider::~InMemoryProvider()
			{

			}

			std::shared_ptr<InMemoryProvider> InMemoryProvider::getInstance()
			{
				static auto one = std::make_shared<InMemoryProvider>();
				return one;
			}

			std::shared_ptr<Abstract> InMemoryProvider::findBlockchain(std::string_view communityId)
			{
				std::lock_guard _lock(mWorkMutex);
				auto it = mBlockchainsPerGroup.find(communityId);
				if (it == mBlockchainsPerGroup.end()) {
					auto insertedIt = mBlockchainsPerGroup.insert({ std::string(communityId), std::make_shared<InMemory>(communityId)});
					if (!insertedIt.second) {
						throw ConstructBlockchainException("cannot insert into mBlockchainsPerGroup", communityId);
					}
					it = insertedIt.first;
				}
				return it->second;
			}
		}
	}

}