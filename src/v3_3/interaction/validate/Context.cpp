#include "gradido_blockchain/v3_3/interaction/validate/Context.h"


namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace validate {
				void Context::run(
					Type type/* = Type::SINGLE*/,
					const std::string& communityId /* = "" */, 
					std::shared_ptr<AbstractBlockchainProvider> blockchainProvider/* = nullptr*/)
				{
					if (!mSenderPreviousConfirmedTransaction) {
						mSenderPreviousConfirmedTransaction = blockchainProvider->findBlockchain(communityId)->getLastTransaction();
					}
					mRole->run(type, communityId, blockchainProvider, mSenderPreviousConfirmedTransaction, mRecipientPreviousConfirmedTransaction);
				}
			}
		}
	}
}