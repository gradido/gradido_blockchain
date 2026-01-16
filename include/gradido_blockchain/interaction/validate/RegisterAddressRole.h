#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_REGISTER_ADDRESS_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_REGISTER_ADDRESS_ROLE_H

#include "AbstractRole.h"

namespace gradido {
	namespace data {
		class RegisterAddress;
	}
	namespace interaction {
		namespace validate {
			class RegisterAddressRole : public AbstractRole
			{
			public:
				RegisterAddressRole(std::shared_ptr<const data::RegisterAddress> registerAddress);

				void run(Type type, ContextData& c);
				void checkRequiredSignatures(
					const data::SignatureMap& signatureMap,
					std::shared_ptr<blockchain::Abstract> blockchain = nullptr
				) const;
			protected:
				std::shared_ptr<const data::RegisterAddress> mRegisterAddress;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_REGISTER_ADDRESS_ROLE_H