#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/data/TransferAmount.h"
#include "gradido_blockchain/interaction/deserialize/TransferAmountRole.h"
#include "gradido_blockchain/interaction/deserialize/Exceptions.h"
#include "gradido_blockchain/lib/DictionaryExceptions.h"
#include "gradido_blockchain/memory/Block.h"

#include <memory>

using std::make_unique, std::make_shared;
using memory::Block;

namespace gradido {
  using data::TransferAmount;

  namespace interaction {
    namespace deserialize {
      TransferAmountRole::TransferAmountRole(const TransferAmountMessage& transferAmount, uint32_t communityIdIndex)
      {
        if (!transferAmount["pubkey"_f].has_value()) {
            throw MissingMemberException("missing member on deserialize transfer amount", "pubkey");
        }
        string communityIdString = transferAmount["community_id"_f].value_or("");
        uint32_t coinCommunityIdIndex = communityIdIndex;
        if (!communityIdString.empty()) {
          auto coinCommunityIdIndexOptional = g_appContext->getCommunityIds().getIndexForData(communityIdString);
          if (!coinCommunityIdIndexOptional.has_value()) {
            throw DictionaryMissingEntryException(
              "missing community id for index in deserialize::TransferAmountRole",
              communityIdString
            );
          }
          coinCommunityIdIndex = coinCommunityIdIndexOptional.value();
        }
        mTransferAmount = make_unique<TransferAmount>(
            make_shared<Block>(transferAmount["pubkey"_f].value()),
            GradidoUnit::fromGradidoCent(transferAmount["amount"_f].value_or(0)),
            coinCommunityIdIndex
        );
      }
    }
  }
}
