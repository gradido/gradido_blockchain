#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/Filter.h"
#include "gradido_blockchain/interaction/calculateAccountBalance/GradidoCreationRole.h"


namespace gradido {
	using namespace blockchain;
	using namespace data;
	namespace interaction {
		namespace calculateAccountBalance {
			GradidoUnit GradidoCreationRole::getAmountAdded(memory::ConstBlockPtr accountPublicKey) const
			{
				if (getRecipient()->isTheSame(accountPublicKey)) {
					return getTransferAmount().getAmount();
				}
				auto firstTransactionEntry = mBlockchain->findOne(Filter::FIRST_TRANSACTION);
				assert(firstTransactionEntry->getTransactionType() == data::TransactionType::COMMUNITY_ROOT);
				auto communityRoot = firstTransactionEntry->getTransactionBody()->getCommunityRoot();
				if (communityRoot->getAufPubkey()->isTheSame(accountPublicKey) || communityRoot->getGmwPubkey()->isTheSame(accountPublicKey)) {
					return getTransferAmount().getAmount();
				}
				return GradidoUnit::zero();
			}
		}
	}
}
