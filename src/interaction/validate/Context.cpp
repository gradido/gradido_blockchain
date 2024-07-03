#include "gradido_blockchain/interaction/validate/Context.h"


namespace gradido {
	namespace interaction {
		namespace validate {
			void Context::run(
				Type type/* = Type::SINGLE*/,
				std::string_view communityId /* = "" */, 
				blockchain::AbstractProvider* blockchainProvider /* = nullptr*/)
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