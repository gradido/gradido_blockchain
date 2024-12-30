#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/FilterBuilder.h"
#include "gradido_blockchain/blockchain/TransactionEntry.h"
#include "gradido_blockchain/interaction/addGradidoTransaction/TransactionBodyRole.h"

using namespace std;

namespace gradido {
    using namespace data;
    using namespace blockchain;

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