#include "gradido_blockchain/v3_3/InMemoryBlockchainProvider.h"

namespace gradido {
	namespace v3_3 {
		InMemoryBlockchainProvider::InMemoryBlockchainProvider()
		{

		}

		InMemoryBlockchainProvider::~InMemoryBlockchainProvider()
		{

		}

		InMemoryBlockchainProvider* InMemoryBlockchainProvider::getInstance()
		{
			static InMemoryBlockchainProvider one;
			return &one;
		}

		std::shared_ptr<AbstractBlockchain> InMemoryBlockchainProvider::findBlockchain(std::string_view communityId)
		{
			
		}

	}

}