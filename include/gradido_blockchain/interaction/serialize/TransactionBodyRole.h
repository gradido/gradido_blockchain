#ifndef GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_TRANSACTION_BODY_ROLE_H
#define GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_TRANSACTION_BODY_ROLE_H

#include "gradido_blockchain/data/CrossGroupType.h"
#include "AbstractRole.h"

namespace gradido {
	namespace data {
		class TransactionBody;
		class TransferAmount;
	}
	namespace interaction {
		namespace serialize {

			class TransactionBodyRole: public AbstractRole
			{
			public:
				TransactionBodyRole(const data::TransactionBody& body) 
					: mBody(body) {}
				~TransactionBodyRole() {};

				memory::ConstBlockPtr run() const override; 
				size_t calculateSerializedSize() const override;

			protected:
				const data::TransactionBody& mBody;
			};
		}
	}
}


#endif // GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_TRANSACTION_BODY_ROLE_H