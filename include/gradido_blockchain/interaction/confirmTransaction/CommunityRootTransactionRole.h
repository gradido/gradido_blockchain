#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CONFIRM_TRANSACTION_COMMUNITY_ROOT_TRANSACTION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CONFIRM_TRANSACTION_COMMUNITY_ROOT_TRANSACTION_ROLE_H

#include "TransactionBodyRole.h"
#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/data/AccountBalance.h"
#include "gradido_blockchain/memory/Block.h"

#include <memory>
#include <vector>

using memory::Block, memory::ConstBlockPtr;
using std::shared_ptr, std::make_shared;
using std::vector;

namespace gradido {
  using data::AccountBalance;
  namespace interaction {
    namespace confirmTransaction {
      class GRADIDOBLOCKCHAIN_EXPORT CommunityRootTransactionRole : public TransactionBodyRole
      {
      public:
        using TransactionBodyRole::TransactionBodyRole;
        virtual validate::Type getValidationType() const { return validate::Type::SINGLE; }
        virtual vector<AccountBalance> calculateAccountBalances(uint64_t maxTransactionNr) const {
          auto communityIdIndex = mBlockchain->getCommunityIdIndex();
          auto communityRoot = mBody->getCommunityRoot().value();
          auto gmwPublicKey = make_shared<const Block>(g_appContext->getPublicKey(communityRoot.gmwPublicKeyIndex).value());
          auto aufPublicKey = make_shared<const Block>(g_appContext->getPublicKey(communityRoot.aufPublicKeyIndex).value());
          return {
              AccountBalance(gmwPublicKey, GradidoUnit::zero(), communityIdIndex),
              AccountBalance(aufPublicKey, GradidoUnit::zero(), communityIdIndex)
          };
        }
      protected:
      };
    }
  }
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_CONFIRM_TRANSACTION_COMMUNITY_ROOT_TRANSACTION_ROLE_H
