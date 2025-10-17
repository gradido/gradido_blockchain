#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_HIERO_ACCOUNT_ID_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_HIERO_ACCOUNT_ID_ROLE_H

#include "gradido_blockchain/export.h"
#include "AbstractRole.h"
#include "Protopuf.h"

namespace hiero {
	class AccountId;
}

namespace gradido {	
	namespace interaction {
		namespace serialize {

			class GRADIDOBLOCKCHAIN_EXPORT HieroAccountIdRole : public AbstractRole
			{
			public:
				HieroAccountIdRole(const hiero::AccountId& accountId) : mAccountId(accountId) {}
				~HieroAccountIdRole() {};

				RUM_IMPLEMENTATION
				HieroAccountIdMessage getMessage() const;
				size_t calculateSerializedSize() const;

			protected:
				const hiero::AccountId& mAccountId;
			};
		}
	}
}

#endif // __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_HIERO_ACCOUNT_ID_ROLE_H