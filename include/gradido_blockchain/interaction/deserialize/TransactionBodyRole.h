#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_TRANSACTION_BODY_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_TRANSACTION_BODY_ROLE_H

#include "Protopuf.h"
#include "gradido_blockchain/TransactionBodyBuilder.h"

namespace gradido {
	namespace interaction {
		namespace deserialize {

			class TransactionBodyRole
			{
			public:
				TransactionBodyRole(const TransactionBodyMessage& bodyMessage);
				~TransactionBodyRole() {};

				inline TransactionBodyBuilder& getBodyBuilder() { return mBodyBuilder; }

			protected:
				TransactionBodyBuilder mBodyBuilder;
			};
		}
	}
}


#endif // __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_TRANSACTION_BODY_ROLE_H