#include "gradido_blockchain/blockchain/AbstractProvider.h"
#include "gradido_blockchain/blockchain/Exceptions.h"
#include "gradido_blockchain/interaction/validate/ConfirmedTransactionRole.h"
#include "gradido_blockchain/interaction/validate/Context.h"
#include "gradido_blockchain/interaction/validate/GradidoTransactionRole.h"
#include "gradido_blockchain/interaction/validate/TransactionBodyRole.h"


namespace gradido {
	namespace interaction {
		namespace validate {

			Context::Context(const data::TransactionBody& body)
				: mRole(std::make_unique<TransactionBodyRole>(body)) 
			{
			}

			Context::Context(const data::GradidoTransaction& body)
				: mRole(std::make_unique<GradidoTransactionRole>(body)) 
			{
			}

			Context::Context(const data::ConfirmedTransaction& body)
				: mRole(std::make_unique<ConfirmedTransactionRole>(body)) 
			{
			}

			Context::~Context()
			{

			}

			void Context::run(Type type/* = Type::SINGLE*/, std::shared_ptr<blockchain::Abstract> blockchain /*= nullptr*/)
			{
				if (!mSenderPreviousConfirmedTransaction && blockchain) {
					auto transactionEntry = blockchain->findOne(blockchain::Filter::LAST_TRANSACTION);
					if (transactionEntry) {
						mSenderPreviousConfirmedTransaction = transactionEntry->getConfirmedTransaction();
					}
				}
				mRole->run(type, blockchain, mSenderPreviousConfirmedTransaction, mRecipientPreviousConfirmedTransaction);
			}
		}
	}
}