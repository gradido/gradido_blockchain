#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/Filter.h"
#include "gradido_blockchain/interaction/calculateAccountBalance/GradidoCreationRole.h"
#include "gradido_blockchain/memory/Block.h"

using memory::Block, memory::ConstBlockPtr;

namespace gradido {
	using namespace blockchain;
	using namespace data;
	namespace interaction {
		namespace calculateAccountBalance {
			GradidoUnit GradidoCreationRole::getAmountAdded(ConstBlockPtr accountPublicKey) const
			{
				if (getRecipient()->isTheSame(accountPublicKey)) {
					return getTransferAmount().getAmount();
				}
				auto firstTransactionEntry = mBlockchain->findOne(Filter::FIRST_TRANSACTION);
				assert(firstTransactionEntry->getTransactionType() == GRDT_TRANSACTION_COMMUNITY_ROOT);
				auto communityRoot = firstTransactionEntry->getTransactionBody()->getCommunityRoot().value();
				assert(accountPublicKey->size() == 32);

				auto accountPublicKeyIndex = g_appContext->getOrAddPublicKeyIndex(mBody->getCommunityIdIndex(), { accountPublicKey->data() });
				if (communityRoot.aufPublicKeyIndex == accountPublicKeyIndex || communityRoot.gmwPublicKeyIndex == accountPublicKeyIndex) {
					return getTransferAmount().getAmount();
				}
				return GradidoUnit::zero();
			}
		}
	}
}
