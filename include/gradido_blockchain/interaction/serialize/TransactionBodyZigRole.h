#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_TRANSACTION_BODY_ZIG_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_TRANSACTION_BODY_ZIG_ROLE_H

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

			class TransactionBodyZigRole: public AbstractRole
			{
			public:
				TransactionBodyZigRole(const data::TransactionBody& body) 
					: mBody(body) {}
				~TransactionBodyZigRole() {};

				memory::ConstBlockPtr run() const override; 
				size_t calculateSerializedSize() const override;

			protected:
				const data::TransactionBody& mBody;
			};
		}
	}
}


#endif // __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_TRANSACTION_BODY_ZIG_ROLE_H