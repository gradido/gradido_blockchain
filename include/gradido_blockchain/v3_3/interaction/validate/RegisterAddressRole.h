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
					RegisterAddressRole(const data::RegisterAddress& registerAddress)
						: mRegisterAddress(registerAddress) {}

					void run(Type type = Type::SINGLE, const std::string& communityId, std::shared_ptr<AbstractBlockchainProvider> blockchainProvider);
				protected:

					const data::RegisterAddress& mRegisterAddress;
				};
			}
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_REGISTER_ADDRESS_ROLE_H