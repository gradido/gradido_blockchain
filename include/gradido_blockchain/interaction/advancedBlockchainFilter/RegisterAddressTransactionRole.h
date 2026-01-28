#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_ADVANCED_BLOCKCHAIN_FILTER_REGISTER_ADDRESS_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_ADVANCED_BLOCKCHAIN_FILTER_REGISTER_ADDRESS_ROLE_H

#include "AbstractTransactionRole.h"
#include "gradido_blockchain/data/adapter/PublicKey.h"

namespace gradido {

	namespace interaction {
		namespace advancedBlockchainFilter {
			class RegisterAddressTransactionRole : public AbstractTransactionRole
			{
			public:
				using AbstractTransactionRole::AbstractTransactionRole;

				memory::ConstBlockPtr getSenderPublicKey() const { return data::adapter::toConstBlockPtr(mBody->getRegisterAddress()->accountPublicKeyIndex); };
				memory::ConstBlockPtr getRecipientPublicKey() const { return nullptr; };

			protected:
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_ADVANCED_BLOCKCHAIN_FILTER_REGISTER_ADDRESS_ROLE_H