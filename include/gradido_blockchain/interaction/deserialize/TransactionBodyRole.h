#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_TRANSACTION_BODY_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_TRANSACTION_BODY_ROLE_H

#include "Protopuf.h"

namespace gradido {
	namespace data {
		class TransactionBody;
	}
	namespace interaction {
		namespace deserialize {

			class TransactionBodyRole
			{
			public:
				TransactionBodyRole(const TransactionBodyMessage& bodyMessage);
				~TransactionBodyRole();

				inline data::TransactionBody& getBody() { return *mTransactionBody; }

			protected:
				std::unique_ptr<data::TransactionBody> mTransactionBody;
			};
		}
	}
}


#endif // __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_TRANSACTION_BODY_ROLE_H