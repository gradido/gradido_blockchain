#ifndef __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_ABSTRACT_PROVIDER_H
#define __GRADIDO_BLOCKCHAIN_BLOCKCHAIN_ABSTRACT_PROVIDER_H

#include "Abstract.h"
#include "gradido_blockchain/lib/Dictionary.h"
#include "gradido_blockchain/memory/Block.h"

#include <memory>
#include <string>

namespace gradido {
	namespace blockchain {
		
		class AbstractProvider
		{
		public:
			AbstractProvider() = default;
			virtual ~AbstractProvider() {}
			// return nullptr if not found
			virtual std::shared_ptr<Abstract> findBlockchain(uint32_t communityIdIndex) = 0;
			// create new if not found
			virtual std::shared_ptr<Abstract> findBlockchain(const std::string& communityId) = 0;
		};

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
