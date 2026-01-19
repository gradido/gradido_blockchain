#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/blockchain/InMemoryProvider.h"
#include "gradido_blockchain/blockchain/Exceptions.h"

#include <string>
#include <string_view>
#include <memory>

using std::shared_ptr, std::make_shared;
using std::string, std::string_view;
using std::shared_lock, std::unique_lock;

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

		shared_ptr<Abstract> InMemoryProvider::findBlockchain(uint32_t communityIdIndex) const
		{
			shared_lock _lock(mWorkMutex);
			if (mBlockchainsPerGroup.size() <= communityIdIndex) {
				throw GradidoNodeInvalidDataException("invalid communityIdIndex, blockchain for this not found, please don't call gradido::g_appContext->addCommunityId");
			}
			return mBlockchainsPerGroup[communityIdIndex];
		}

		shared_ptr<Abstract> InMemoryProvider::findBlockchain(const string& communityId)
		{
			auto& communityIdsDictionary = g_appContext->getCommunityIds();
			auto communityIdIndex = communityIdsDictionary.getIndexForData(communityId);
			if (communityIdIndex.has_value()) {
				return findBlockchain(communityIdIndex.value());
			}
			unique_lock _lock(mWorkMutex);
			auto freshIndex = g_appContext->addCommunityId(communityId);
			if (freshIndex != mBlockchainsPerGroup.size()) {
				throw GradidoNodeInvalidDataException("mBlockchainsPerGroup.size don't is identical to new communityIdIndex");
			}
			auto blockchain = new InMemory(communityId, freshIndex);
			mBlockchainsPerGroup.emplace_back(blockchain);
			return mBlockchainsPerGroup[freshIndex];
		}

		void InMemoryProvider::clear()
		{
			unique_lock _lock(mWorkMutex);
			mBlockchainsPerGroup.clear();
		}
	}
}