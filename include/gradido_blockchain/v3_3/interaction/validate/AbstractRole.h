#ifndef __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_ABSTRACT_ROLE_H
#define __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_ABSTRACT_ROLE_H

#include "Type.h"
#include "gradido_blockchain/v3_3/AbstractBlockchainProvider.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace validate {

                class AbstractRole 
                {
                public:
                  virtual ~AbstractRole() {}
                  virtual void prepare() {};
                  // test if transaction is valid, throw an exception on error
                  virtual void run(Type type = Type::SINGLE, const std::string& communityId = "", std::shared_ptr<AbstractBlockchainProvider> blockchainProvider = nullptr) = 0;

                protected:
                    bool isValidCommunityAlias(const std::string& communityAlias);

                };
            }
        }
    }
}



#endif //__GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_ABSTRACT_ROLE_H