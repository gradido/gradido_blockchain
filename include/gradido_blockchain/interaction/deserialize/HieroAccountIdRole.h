#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_HIERO_ACCOUNT_ID_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_HIERO_ACCOUNT_ID_ROLE_H

#include "gradido_blockchain/data/hiero/AccountId.h"
#include "gradido_blockchain/export.h"
#include "Protopuf.h"
#include <memory>

namespace gradido {
    namespace interaction {
        namespace deserialize {
            class GRADIDOBLOCKCHAIN_EXPORT HieroAccountIdRole
            {
            public:
                HieroAccountIdRole(const HieroAccountIdMessage& hieroAccountId);
                ~HieroAccountIdRole();
                
                inline operator const hiero::AccountId& () const { return mAccountId; }
                inline const hiero::AccountId& data() const { return mAccountId; }
            protected:
                hiero::AccountId mAccountId;
            };
        }
    }
}

#endif // __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_HIERO_ACCOUNT_ID_ROLE_H