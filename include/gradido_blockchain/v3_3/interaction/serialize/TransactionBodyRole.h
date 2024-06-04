#ifndef __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_SERIALIZE_TRANSACTION_BODY_ROLE_H
#define __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_SERIALIZE_TRANSACTION_BODY_ROLE_H

#include "gradido_blockchain/v3_3/data/Protocol.h"
#include "gradido_blockchain/v3_3/data/Protopuf.h"
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

					virtual memory::ConstBlockPtr run() const;

				protected:
					//data::TransferAmountMessage getTransferAmountMessage(const data::TransferAmount& amount) const;
					//data::GradidoTransferMessage getTransferMessage(const data::GradidoTransfer& transfer) const;
					//data::RegisterAddressMessage getRegisterAddressMessage(const data::RegisterAddress& registerAddress) const;

					const data::TransactionBody& mBody;
				};
			}
		}
	}
}


#endif // __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_SERIALIZE_TRANSACTION_BODY_ROLE_H