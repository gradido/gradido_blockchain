#include "gradido_blockchain/v3_3/interaction/validate/Context.h"


namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace validate {
				void Context::run(
					Type type/* = Type::SINGLE*/,
					std::string_view communityId /* = "" */, 
					std::shared_ptr<blockchain::AbstractProvider> blockchainProvider/* = nullptr*/)
				{
					if (!mSenderPreviousConfirmedTransaction) {
						auto transactionEntry = blockchainProvider->findBlockchain(communityId)->findOne(blockchain::Filter::LAST_TRANSACTION);
						mSenderPreviousConfirmedTransaction = transactionEntry->getConfirmedTransaction();
					}
					mRole->run(type, communityId, blockchainProvider, mSenderPreviousConfirmedTransaction, mRecipientPreviousConfirmedTransaction);
				}
			}
		}
	}
}