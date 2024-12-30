#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_GRADIDO_TRANSFER_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_GRADIDO_TRANSFER_ROLE_H

#include "AbstractRole.h"

namespace gradido {
	namespace data {
		class TransferAmount;
	}
	namespace interaction {
		namespace calculateAccountBalance {
			class GradidoTransferRole : public AbstractRole
			{
			public:
				using AbstractRole::AbstractRole;

				virtual memory::ConstBlockPtr getRecipient() const {
					return mBody->getTransfer()->getRecipient();
				}
			protected:

			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_GRADIDO_TRANSFER_ROLE_H