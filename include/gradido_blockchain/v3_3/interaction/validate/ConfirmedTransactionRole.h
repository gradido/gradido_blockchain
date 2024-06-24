#ifndef __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_CONFIRMED_TRANSACTION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_CONFIRMED_TRANSACTION_ROLE_H

#include "AbstractRole.h"
#include "gradido_blockchain/v3_3/data/Protocol.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace validate {
				class ConfirmedTransactionRole : public AbstractRole
				{
				public:
					ConfirmedTransactionRole(const data::ConfirmedTransaction& confirmedTransaction)
						: mConfirmedTransaction(confirmedTransaction) {}

					void run(
						Type type,
						std::string_view communityId,
						std::shared_ptr<blockchain::AbstractProvider> blockchainProvider,
						data::ConstConfirmedTransactionPtr previousConfirmedTransaction,
						data::ConstConfirmedTransactionPtr recipientPreviousConfirmedTransaction
					);
				protected:

					const data::ConfirmedTransaction& mConfirmedTransaction;
				};
			}
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_CONFIRMED_TRANSACTION_ROLE_H