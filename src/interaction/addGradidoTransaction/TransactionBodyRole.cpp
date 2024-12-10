#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/interaction/addGradidoTransaction/TransactionBodyRole.h"


namespace gradido {
    using namespace data;

    namespace interaction {
        namespace addGradidoTransaction {

            TransactionBodyRole::TransactionBodyRole(
                std::shared_ptr<data::GradidoTransaction> gradidoTransaction,
                memory::ConstBlockPtr messageId,
                Timepoint confirmedAt
            ) : AbstractRole(gradidoTransaction, messageId, confirmedAt), 
                mBody(gradidoTransaction->getTransactionBody()) 
            {
            }

            validate::Type TransactionBodyRole::getValidationType() const
            {
                auto type = validate::Type::SINGLE | validate::Type::ACCOUNT;
                if (mBody->getType() != data::CrossGroupType::LOCAL) {
                    type = type | validate::Type::PAIRED;
                }
                return type;
            }
        }
    }
}