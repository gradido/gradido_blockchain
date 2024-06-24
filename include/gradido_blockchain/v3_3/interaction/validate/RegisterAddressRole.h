#ifndef __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_REGISTER_ADDRESS_ROLE_H
#define __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_REGISTER_ADDRESS_ROLE_H

#include "AbstractRole.h"
#include "gradido_blockchain/v3_3/data/Protocol.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace validate {
				class RegisterAddressRole : public AbstractRole
				{
				public:
					RegisterAddressRole(const data::RegisterAddress& registerAddress);

					void run(
						Type type,
						std::string_view communityId,
						std::shared_ptr<blockchain::AbstractProvider> blockchainProvider,
						data::ConstConfirmedTransactionPtr senderPreviousConfirmedTransaction,
						data::ConstConfirmedTransactionPtr recipientPreviousConfirmedTransaction
					);
				protected:
					const data::RegisterAddress& mRegisterAddress;
				};
			}
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_REGISTER_ADDRESS_ROLE_H