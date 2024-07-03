#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_TRANSACTION_BODY_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_TRANSACTION_BODY_ROLE_H

#include "gradido_blockchain/data/Protocol.h"
#include "Protopuf.h"
#include "gradido_blockchain/data/CrossGroupType.h"

namespace gradido {
	namespace interaction {
		namespace deserialize {

			class TransactionBodyRole
			{
			public:
				TransactionBodyRole(const TransactionBodyMessage& bodyMessage);
				~TransactionBodyRole() {};

				inline data::ConstTransactionBodyPtr getTransactionBody() { return mBody; }

			protected:
				std::shared_ptr<data::TransactionBody> mBody;
			};
		}
	}
}


#endif // __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_TRANSACTION_BODY_ROLE_H