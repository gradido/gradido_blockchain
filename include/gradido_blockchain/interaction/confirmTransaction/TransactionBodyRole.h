#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CONFIRM_TRANSACTION_TRANSACTION_BODY_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CONFIRM_TRANSACTION_TRANSACTION_BODY_ROLE_H

#include "AbstractRole.h"

namespace gradido {
    namespace data {
        class TransactionBody;
    }
    namespace interaction {
        namespace confirmTransaction {
            class GRADIDOBLOCKCHAIN_EXPORT TransactionBodyRole : public AbstractRole
            {
            public:
                TransactionBodyRole(
                    std::shared_ptr<const data::GradidoTransaction> gradidoTransaction,
                    memory::ConstBlockPtr messageId, 
                    data::Timestamp confirmedAt,
                    std::shared_ptr<blockchain::Abstract> blockchain
                );
                virtual validate::Type getValidationType() const;
            protected:
                std::shared_ptr<const data::TransactionBody> mBody;
            };
        }
    }
}
#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_CONFIRM_TRANSACTION_TRANSACTION_BODY_ROLE_H