#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/blockchain/InMemoryProvider.h"
#include "gradido_blockchain/blockchain/Exceptions.h"
#include "gradido_blockchain/lib/DictionaryExceptions.h"

#include <memory>
#include <optional>
#include <string>
#include <string_view>

using std::optional, std::nullopt;
using std::shared_ptr, std::make_shared;
using std::string, std::string_view, std::to_string;
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

		shared_ptr<Abstract> InMemoryProvider::findBlockchain(uint32_t communityIdIndex) 
		{
			{
				shared_lock _lock(mWorkMutex);
				auto it = mBlockchainsPerGroup.find(communityIdIndex);
				if (it != mBlockchainsPerGroup.end()) {
					return it->second;
				}
			}
			return addBlockchain("", communityIdIndex);
		}

		shared_ptr<Abstract> InMemoryProvider::findBlockchain(const string& communityId)
		{
			auto communityIdIndex = g_appContext->getOrAddCommunityIdIndex(communityId);
			{
				shared_lock _lock(mWorkMutex);
				auto it = mBlockchainsPerGroup.find(communityIdIndex);
				if (it != mBlockchainsPerGroup.end()) {
					return it->second;
				}
			}
			return addBlockchain(communityId, communityIdIndex);
		}

		shared_ptr<Abstract> InMemoryProvider::addBlockchain(const string& communityId, optional<uint32_t> communityIdIndex)
		{
			string_view communityIdValue;
			uint32_t communityIdIndexValue = 0;
			// we got both values
			if (!communityId.empty() && communityIdIndex.has_value()) {
				communityIdValue = communityId;
				communityIdIndexValue = communityIdIndex.value();
			} // we need to resolve the community id string
			else if (communityId.empty() && communityIdIndex.has_value()) {
				auto communityIdOptional = g_appContext->getCommunityIds().getDataForIndex(communityIdIndex.value());
				if (!communityIdOptional.has_value()) {
					throw DictionaryMissingEntryException("cannot find community id for addBlockchain", to_string(communityIdIndex.value()));
				}
				communityIdValue = communityIdOptional.value();
				communityIdIndexValue = communityIdIndex.value();
			}  // we need to resolve the community id index
			else if (!communityId.empty() && !communityIdIndex.value()) {
				communityIdIndexValue = g_appContext->getOrAddCommunityIdIndex(communityId);
				communityIdValue = communityId;
			}
			else {
				throw GradidoNodeInvalidDataException("please call addBlockchain either with a value for communityId or for communityIdIndex");
			}
			unique_lock _lock(mWorkMutex);
			// cannot use make_shared here, because InMemory has private constructor
			// TODO: maybe use enable_shared_from_this like blockchain::FileBased in GradidoNode
			auto result = mBlockchainsPerGroup.insert({ communityIdIndexValue, shared_ptr<InMemory>(new InMemory(communityIdValue, communityIdIndexValue)) });
			return result.first->second;
		}

		void InMemoryProvider::clear()
		{
			unique_lock _lock(mWorkMutex);
			mBlockchainsPerGroup.clear();
		}
	}
}