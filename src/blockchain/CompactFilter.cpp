#include "gradido_blockchain/blockchain/CompactFilter.h"
#include "gradido_blockchain/data/adapter/publicKey.h"

namespace gradido {
  using data::adapter::toPublicKey;
  namespace blockchain {
    CompactFilter::CompactFilter()
      : minTransactionNr(0), maxTransactionNr(0),
      publicKeyIndex(0), coinCommunityIdIndex(0),
      transactionType(data::TransactionType::NONE), searchDirection(SearchDirection::DESC), publicKeySearchType(PublicKeySearchType::None),
      hasCoinCommunityIndex(false)
    {
      
    }

    CompactFilter::CompactFilter(const Filter& filter, const IDictionary<PublicKey>& publicKeyDictionary)
      : minTransactionNr(filter.minTransactionNr), maxTransactionNr(filter.maxTransactionNr),
      publicKeyIndex(0), coinCommunityIdIndex(0),
      transactionType(filter.transactionType), searchDirection(filter.searchDirection), publicKeySearchType(PublicKeySearchType::None),
      hasCoinCommunityIndex(false), pagination(filter.pagination)
    {
      if (filter.updatedBalancePublicKey && !filter.updatedBalancePublicKey->isEmpty()) {
        auto publicKeyIndexOptional = publicKeyDictionary.getIndexForData(toPublicKey(filter.updatedBalancePublicKey));
        if (publicKeyIndexOptional && static_cast<uint32_t>(*publicKeyIndexOptional) == *publicKeyIndexOptional) {
          publicKeyIndex = *publicKeyIndexOptional;
          publicKeySearchType = PublicKeySearchType::BalanceChangingPublicKey;
        }
        else {
          publicKeySearchType = PublicKeySearchType::MissingIndex;
        }
      }
      if (filter.involvedPublicKey && !filter.involvedPublicKey->isEmpty()) {
        if (PublicKeySearchType::None != publicKeySearchType) {
          throw GradidoNodeInvalidDataException("couldn't handle involvedPublicKey and updatedBalancePublicKey in filter");
        }
        auto publicKeyIndexOptional = publicKeyDictionary.getIndexForData(toPublicKey(filter.involvedPublicKey));
        if (publicKeyIndexOptional && static_cast<uint32_t>(*publicKeyIndexOptional) == *publicKeyIndexOptional) {
          publicKeyIndex = *publicKeyIndexOptional;
          publicKeySearchType = PublicKeySearchType::InvolvedPublicKey;
        }
        else {
          publicKeySearchType = PublicKeySearchType::MissingIndex;
        }
      }
      if (filter.coinCommunityIdIndex) {
        coinCommunityIdIndex = *filter.coinCommunityIdIndex;
        hasCoinCommunityIndex = true;
      }
    }
  }
}