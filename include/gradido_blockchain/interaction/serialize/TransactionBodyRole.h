#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_TRANSACTION_BODY_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_TRANSACTION_BODY_ROLE_H

#include "gradido_blockchain/data/CrossGroupType.h"
#include "AbstractRole.h"
#include "Protopuf.h"

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
				TransactionBodyRole(const data::TransactionBody& body) : mBody(body) {}
				~TransactionBodyRole() {};

				RUM_IMPLEMENTATION
				TransactionBodyMessage getMessage() const;
				size_t calculateSerializedSize() const;		

			protected:
				size_t calculateTransferAmountSerializedSize(const data::TransferAmount& amount) const;
				TransferAmountMessage createTransferAmountMessage(const data::TransferAmount& amount) const;
				const data::TransactionBody& mBody;
			};
		}
	}
}


#endif // __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_TRANSACTION_BODY_ROLE_H