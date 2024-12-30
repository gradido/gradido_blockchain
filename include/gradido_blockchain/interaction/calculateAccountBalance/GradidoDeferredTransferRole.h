#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_GRADIDO_DEFERRED_TRANSFER_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_GRADIDO_DEFERRED_TRANSFER_ROLE_H

#include "AbstractRole.h"

namespace gradido {
	namespace interaction {
		namespace calculateAccountBalance {
			class GradidoDeferredTransferRole : public AbstractRole
			{
			public:
				using AbstractRole::AbstractRole;

				virtual memory::ConstBlockPtr getRecipient() const {
					return mBody->getDeferredTransfer()->getTransfer().getRecipient();
				}

			protected:						
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_ACCOUNT_BALANCE_GRADIDO_DEFERRED_TRANSFER_ROLE_H