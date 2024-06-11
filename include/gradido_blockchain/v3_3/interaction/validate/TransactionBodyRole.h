#ifndef __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_TRANSACTION_BODY_ROLE_H
#define __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_TRANSACTION_BODY_ROLE_H

#include "AbstractRole.h"
#include "gradido_blockchain/v3_3/data/Protocol.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace validate {
        class TransactionBodyRole: public AbstractRole 
        {
        public:
          TransactionBodyRole(const data::TransactionBody& body): mBody(body) {}
          void run();

        protected:
          const data::TransactionBody& mBody;

        };
      }
    }
  }
}



#endif //__GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_TRANSACTION_BODY_ROLE_H