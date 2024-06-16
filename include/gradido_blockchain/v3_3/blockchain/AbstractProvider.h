#ifndef __GRADIDO_BLOCKCHAIN_V3_3_BLOCKCHAIN_ABSTRACT_PROVIDER_H
#define __GRADIDO_BLOCKCHAIN_V3_3_BLOCKCHAIN_ABSTRACT_PROVIDER_H

#include "Abstract.h"

namespace gradido {
	namespace v3_3 {
		namespace blockchain {
			class AbstractBlockchainProvider
			{
			public:
				virtual ~AbstractBlockchainProvider() {}
				virtual std::shared_ptr<AbstractBlockchain> findBlockchain(std::string_view communityId) = 0;
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
}

#endif //__GRADIDO_BLOCKCHAIN_V3_3_BLOCKCHAIN_ABSTRACT_PROVIDER_H
