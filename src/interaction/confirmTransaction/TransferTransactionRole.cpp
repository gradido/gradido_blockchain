#include "gradido_blockchain/interaction/confirmTransaction/TransferTransactionRole.h"
#include "gradido_blockchain/data/AccountBalance.h"
#include "gradido_blockchain/data/TransactionBody.h"
#include "gradido_blockchain_core/types/cross_group.h"

#include <magic_enum/magic_enum.hpp>
#include <vector>

using namespace magic_enum;
using std::vector;

namespace gradido {
  using namespace blockchain;
  using data::AccountBalance;

  namespace interaction::confirmTransaction {
    vector<AccountBalance> TransferTransactionRole::calculateAccountBalances(uint64_t maxTransactionNr) const
    {
      auto transfer = mBody->getTransfer();
      auto& transferAmount = transfer->getSender();
      auto coinCommunityIdIndex = transferAmount.getCoinCommunityIdIndex();

      switch (mBody->getType()) {
      case GRDT_CROSS_GROUP_LOCAL:
        return {
          // sender
          calculateAccountBalance(transferAmount.getPublicKey(), maxTransactionNr, transferAmount.getAmount().negated(), coinCommunityIdIndex),
          // recipient
          calculateAccountBalance(transfer->getRecipient(), maxTransactionNr, transferAmount.getAmount(), coinCommunityIdIndex)
        };
      case GRDT_CROSS_GROUP_OUTBOUND:
        return {
          // sender
          calculateAccountBalance(transferAmount.getPublicKey(), maxTransactionNr, transferAmount.getAmount().negated(), coinCommunityIdIndex),
        };
      case GRDT_CROSS_GROUP_INBOUND:
        return {
          // recipient
          calculateAccountBalance(transfer->getRecipient(), maxTransactionNr, transferAmount.getAmount(), coinCommunityIdIndex)
        };
      default: 
        throw GradidoUnhandledEnum(
          "interaction::confirmTransaction transfer account balance", 
          "grdt_cross_group", 
          enum_name(mBody->getType()).data()
        );
      }
    }
  }
}
