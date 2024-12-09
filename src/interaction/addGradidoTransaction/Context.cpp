#include "gradido_blockchain/interaction/addGradidoTransaction/Context.h"
#include "gradido_blockchain/interaction/validate/Context.h"
#include "gradido_blockchain/blockchain/Abstract.h"

namespace gradido {
	using namespace data;

    namespace interaction {
        namespace addGradidoTransaction {

            bool Context::run(std::shared_ptr<const data::GradidoTransaction> gradidoTransaction, memory::ConstBlockPtr messageId, Timepoint confirmedAt)
            {
				auto provider = mBlockchain->getProvider();
				auto communityId = mBlockchain->getCommunityId();

				interaction::validate::Context validateGradidoTransaction(*gradidoTransaction);
				validateGradidoTransaction.run(interaction::validate::Type::SINGLE, communityId, provider);
				
				if (isTransactionExist(gradidoTransaction)) {
					return false;
				}
				uint64_t id = 1;
				auto lastTransaction = findOne(Filter::LAST_TRANSACTION);
				if (lastTransaction) {
					id = lastTransaction->getTransactionNr() + 1;
				}
				else {
					mStartDate = gradidoTransaction->getTransactionBody()->getCreatedAt();
				}

				auto serializedTransaction = gradidoTransaction->getSerializedTransaction();
				auto body = gradidoTransaction->getTransactionBody();
				if (!messageId) {
					// fake message id simply with taking hash from serialized transaction,
					// iota message id will normally calculated with same algorithmus but with additional data 
					messageId = std::make_shared<memory::Block>(serializedTransaction->calculateHash());
				}

				interaction::calculateAccountBalance::Context finalBalanceCalculate(*this);
				auto finalBalance = finalBalanceCalculate.run(gradidoTransaction, confirmedAt, id);

				data::ConstConfirmedTransactionPtr lastConfirmedTransaction;
				if (lastTransaction) {
					lastConfirmedTransaction = lastTransaction->getConfirmedTransaction();
					if (confirmedAt < lastConfirmedTransaction->getConfirmedAt().getAsTimepoint()) {
						throw BlockchainOrderException("previous transaction is younger");
					}
				}

				auto confirmedTransaction = std::make_shared<data::ConfirmedTransaction>(
					id,
					gradidoTransaction,
					confirmedAt,
					GRADIDO_CONFIRMED_TRANSACTION_V3_3_VERSION_STRING,
					messageId,
					finalBalance,
					lastConfirmedTransaction
				);

				// important! validation
				interaction::validate::Context validate(*confirmedTransaction);
				interaction::validate::Type level =
					// interaction::validate::Type::SINGLE | // already checked
					interaction::validate::Type::PREVIOUS |
					interaction::validate::Type::MONTH_RANGE |
					interaction::validate::Type::ACCOUNT
					;
				if (body->getType() != data::CrossGroupType::LOCAL) {
					level = level | interaction::validate::Type::PAIRED;
				}
				// throw if some error occure
				validate.run(level, getCommunityId(), provider);

				auto transactionEntry = std::make_shared<TransactionEntry>(confirmedTransaction);
				pushTransactionEntry(transactionEntry);
				mTransactionFingerprintTransactionEntry.insert({ *confirmedTransaction->getGradidoTransaction()->getFingerprint(), transactionEntry });

				return true;
            }
        }
    }
}