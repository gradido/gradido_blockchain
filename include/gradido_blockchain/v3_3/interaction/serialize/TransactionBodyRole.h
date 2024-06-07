#ifndef __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_SERIALIZE_TRANSACTION_BODY_ROLE_H
#define __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_SERIALIZE_TRANSACTION_BODY_ROLE_H

#include "gradido_blockchain/v3_3/data/Protocol.h"
#include "gradido_blockchain/v3_3/data/Protopuf.h"
#include "gradido_blockchain/v3_3/data/CrossGroupType.h"
#include "AbstractRole.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace serialize {

				class TransactionBodyRole: public AbstractRole
				{
				public:
					TransactionBodyRole(const data::TransactionBody& body) : mBody(body) {}
					~TransactionBodyRole() {};

					RUM_IMPLEMENTATION
					data::TransactionBodyMessage getMessage() const;
					size_t calculateSerializedSize() const;					
					size_t calculateTransferAmountSerializedSize(const data::TransferAmount& amount) const;

				protected:
					const data::TransactionBody& mBody;
				};
			}
		}
	}
}


#endif // __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_SERIALIZE_TRANSACTION_BODY_ROLE_H