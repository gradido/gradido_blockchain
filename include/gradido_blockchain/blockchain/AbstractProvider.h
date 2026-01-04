#ifndef __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_ABSTRACT_PROVIDER_H
#define __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_ABSTRACT_PROVIDER_H

#include "Abstract.h"
#include "gradido_blockchain/lib/Dictionary.h"
#include "gradido_blockchain/memory/Block.h"

#include <memory>

namespace gradido {
	namespace blockchain {
		
		class AbstractProvider
		{
		public:
			AbstractProvider() : mCommunityIdDicitionary("CommunityId") {}
			virtual ~AbstractProvider() {}
			virtual std::shared_ptr<Abstract> findBlockchain(std::string_view communityId) = 0;

			// access community id index
			inline uint32_t getCommunityIdIndex(const std::string& communityId);
			inline uint32_t getCommunityIdIndex(std::string_view communityId);
			inline const std::string getCommunityIdString(uint32_t index);
		protected: 
			// TODO: move into overall provider if different providers in a system should be supported at the same time
			StringRuntimeDictionary mCommunityIdDicitionary;
		};

		uint32_t AbstractProvider::getCommunityIdIndex(const std::string& communityId)
		{
			return mCommunityIdDicitionary.getIndexForData(communityId).value();
		}

		uint32_t AbstractProvider::getCommunityIdIndex(std::string_view communityId)
		{
			return getCommunityIdIndex(std::string(communityId));
		}

		const std::string AbstractProvider::getCommunityIdString(uint32_t index)
		{
			return mCommunityIdDicitionary.getDataForIndex(index).value();
		}

		class GroupNotFoundException : public GradidoBlockchainException
		{
		public:
			explicit GroupNotFoundException(const char* what, const std::string& communityId) noexcept
				: GradidoBlockchainException(what), mCommunityId(communityId) {}
			std::string getFullString() const;
		protected:
			std::string mCommunityId;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_BLOCKCHAIN_ABSTRACT_PROVIDER_H
