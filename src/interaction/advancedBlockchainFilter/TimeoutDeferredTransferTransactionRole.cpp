#include "gradido_blockchain/interaction/advancedBlockchainFilter/TimeoutDeferredTransferTransactionRole.h"
#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

namespace gradido {
	namespace interaction {
		namespace advancedBlockchainFilter {
			TimeoutDeferredTransferTransactionRole::TimeoutDeferredTransferTransactionRole(
				std::shared_ptr<const data::TransactionBody> body,
				std::shared_ptr<blockchain::Abstract> blockchain
			) : AbstractTransactionRole(body)
			{
				auto timeoutDeferredTransfer = mBody->getTimeoutDeferredTransfer();
				auto deferredTransactionEntry = blockchain->getTransactionForId(timeoutDeferredTransfer->getDeferredTransferTransactionNr());
				mDeferredTransferBody = deferredTransactionEntry->getTransactionBody();
				if (!mDeferredTransferBody) {
					throw GradidoNullPointerException("missing deferred Transfer body", "data::TransactionBody", __FUNCTION__);
				}
			}
		}
	}
}