#include "gradido_blockchain/interaction/validate/Context.h"
#include "gradido_blockchain/blockchain/Exceptions.h"

namespace gradido {
	namespace interaction {
		namespace validate {
			void Context::run(
				Type type/* = Type::SINGLE*/,
				std::string_view communityId /* = "" */, 
				blockchain::AbstractProvider* blockchainProvider /* = nullptr*/)
			{
				if (!mSenderPreviousConfirmedTransaction && blockchainProvider) {
					auto blockchain = blockchainProvider->findBlockchain(communityId);
					if (!blockchain) {
						throw blockchain::CommunityNotFoundException("missing blockchain for interaction::validate", communityId);
					}
					auto transactionEntry = blockchain->findOne(blockchain::Filter::LAST_TRANSACTION);
					if (transactionEntry) {
						mSenderPreviousConfirmedTransaction = transactionEntry->getConfirmedTransaction();
					}
				}
				mRole->run(type, communityId, blockchainProvider, mSenderPreviousConfirmedTransaction, mRecipientPreviousConfirmedTransaction);
			}
		}
	}
}