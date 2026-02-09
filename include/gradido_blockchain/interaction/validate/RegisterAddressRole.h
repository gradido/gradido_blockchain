#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_REGISTER_ADDRESS_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_REGISTER_ADDRESS_ROLE_H

#include "AbstractRole.h"
#include "gradido_blockchain/data/compact/RegisterAddressTx.h"

namespace gradido::interaction::validate {
	class RegisterAddressRole : public AbstractRole
	{
	public:
		RegisterAddressRole(const data::compact::RegisterAddressTx& registerAddress);

		void run(Type type, ContextData& c);
		void checkRequiredSignatures(
			const data::SignatureMap& signatureMap,
			std::shared_ptr<blockchain::Abstract> blockchain = nullptr
		) const;
	protected:
		data::compact::RegisterAddressTx mRegisterAddress;
	};
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_REGISTER_ADDRESS_ROLE_H