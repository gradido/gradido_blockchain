#ifndef __GRADIDO_BLOCKCHAIN_V3_3_ABSTRACT_BLOCKCHAIN_PROVIDER_H
#define __GRADIDO_BLOCKCHAIN_V3_3_ABSTRACT_BLOCKCHAIN_PROVIDER_H

#include "AbstractBlockchain.h"

namespace gradido {
	namespace v3_3 {
		class AbstractBlockchainProvider
		{
		public:
			virtual ~AbstractBlockchainProvider() {}
			virtual std::shared_ptr<AbstractBlockchain> findBlockchain(const std::string& communityId) = 0;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_V3_3_ABSTRACT_BLOCKCHAIN_PROVIDER_H
