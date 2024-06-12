#ifndef __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_CONTEXT_H
#define __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_CONTEXT_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/v3_3/data/Protocol.h"
#include "TransactionBodyRole.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace validate {
                class GRADIDOBLOCKCHAIN_EXPORT Context 
                {
                public:
                    //! \param blockchainProvider provide Blockchain access to search for specific transactions
                  Context(const data::TransactionBody& body, const std::string& communityId = "", std::shared_ptr<AbstractBlockchainProvider> blockchainProvider = nullptr)
                      : mRole(std::make_unique<TransactionBodyRole>(body)), mCommunityId(communityId), mBlockchainProvider(blockchainProvider) {}

                  inline void run(Type type = Type::SINGLE) {
                      mRole->run(type);
                  }
                protected:
                    std::unique_ptr<AbstractRole> mRole;
                    std::string mCommunityId;
                    std::shared_ptr<AbstractBlockchainProvider> mBlockchainProvider;
                };
      }
    }
  }
}



#endif //__GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_CONTEXT_H