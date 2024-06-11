#ifndef __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_CONTEXT_H
#define __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_CONTEXT_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/v3_3/data/Protocol.h"
#include "AbstractRole.h"
#include "Type.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace validate {
        class GRADIDOBLOCKCHAIN_EXPORT Context 
        {
        public:
          Context(const data::TransactionBody& body)

        protected:

        };
      }
    }
  }
}



#endif //__GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_CONTEXT_H