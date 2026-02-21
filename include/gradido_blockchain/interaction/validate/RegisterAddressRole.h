#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_REGISTER_ADDRESS_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_REGISTER_ADDRESS_ROLE_H

#include "AbstractRole.h"

namespace gradido {
	namespace interaction::validate {
		struct RegisterAddressTx;
		class RegisterAddressRole : public AbstractRole
		{
		public:
			RegisterAddressRole(const data::compact::RegisterAddressTx& registerAddress, uint32_t communityIdIndex);

			void run(Type type, ContextData& c);
			void checkRequiredSignatures(
				const data::SignatureMap& signatureMap,
				std::shared_ptr<blockchain::Abstract> blockchain = nullptr
			) const;
		protected:
			data::compact::RegisterAddressTx mRegisterAddress;
			uint32_t mCommunityIdIndex;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_REGISTER_ADDRESS_ROLE_H