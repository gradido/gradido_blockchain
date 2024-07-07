#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_REGISTER_ADDRESS_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_REGISTER_ADDRESS_ROLE_H

#include "AbstractRole.h"
#include "gradido_blockchain/data/Protocol.h"

namespace gradido {
	namespace interaction {
		namespace validate {
			class RegisterAddressRole : public AbstractRole
			{
			public:
				RegisterAddressRole(std::shared_ptr<const data::RegisterAddress> registerAddress);

				void run(
					Type type,
					std::string_view communityId,
					blockchain::AbstractProvider* blockchainProvider,
					data::ConstConfirmedTransactionPtr senderPreviousConfirmedTransaction,
					data::ConstConfirmedTransactionPtr recipientPreviousConfirmedTransaction
				);
			protected:
				std::shared_ptr<const data::RegisterAddress> mRegisterAddress;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_REGISTER_ADDRESS_ROLE_H