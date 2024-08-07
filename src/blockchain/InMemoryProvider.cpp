#include "gradido_blockchain/blockchain/InMemoryProvider.h"
#include "gradido_blockchain/blockchain/Exceptions.h"

namespace gradido {
	namespace blockchain {
		InMemoryProvider::InMemoryProvider()
		{

		}

		InMemoryProvider::~InMemoryProvider()
		{

		}

		InMemoryProvider* InMemoryProvider::getInstance()
		{
			static InMemoryProvider one;
			return &one;
		}

		std::shared_ptr<Abstract> InMemoryProvider::findBlockchain(std::string_view communityId)
		{
			std::lock_guard _lock(mWorkMutex);
			auto it = mBlockchainsPerGroup.find(communityId);
			if (it == mBlockchainsPerGroup.end()) {
				std::shared_ptr<InMemory> blockchain(new InMemory(communityId));
				auto insertedIt = mBlockchainsPerGroup.insert({ std::string(communityId), blockchain });
				if (!insertedIt.second) {
					throw ConstructBlockchainException("cannot insert into mBlockchainsPerGroup", communityId);
				}
				it = insertedIt.first;
			}
			return it->second;
		}

		void InMemoryProvider::clear()
		{
			std::lock_guard _lock(mWorkMutex);
			mBlockchainsPerGroup.clear();
		}
	}
}