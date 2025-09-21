#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/FilterBuilder.h"
#include "gradido_blockchain/blockchain/TransactionEntry.h"
#include "gradido_blockchain/interaction/confirmTransaction/TransactionBodyRole.h"

using namespace std;

namespace gradido {
    using namespace data;
    using namespace blockchain;

    namespace interaction {
        namespace confirmTransaction {

            TransactionBodyRole::TransactionBodyRole(
                std::shared_ptr<const data::GradidoTransaction> gradidoTransaction,
                memory::ConstBlockPtr messageId,
                Timestamp confirmedAt,
                std::shared_ptr<blockchain::Abstract> blockchain
            ) : AbstractRole(gradidoTransaction, messageId, confirmedAt, blockchain),
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