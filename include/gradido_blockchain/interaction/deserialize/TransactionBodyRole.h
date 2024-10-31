#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_TRANSACTION_BODY_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_TRANSACTION_BODY_ROLE_H

#include "Protopuf.h"
#include "gradido_blockchain/data/TransactionBody.h"

namespace gradido {
	namespace data {
		TransactionBody;
	}
	namespace interaction {
		namespace deserialize {

			class TransactionBodyRole
			{
			public:
				TransactionBodyRole(const TransactionBodyMessage& bodyMessage);
				~TransactionBodyRole() {};

				inline data::TransactionBody& getBody() { return *mTransactionBody; }

			protected:
				std::unique_ptr<data::TransactionBody> mTransactionBody;
			};
		}
	}
}


#endif // __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_TRANSACTION_BODY_ROLE_H