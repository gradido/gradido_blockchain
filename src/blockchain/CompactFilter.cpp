#include "gradido_blockchain/blockchain/CompactFilter.h"
#include "gradido_blockchain/data/adapter/publicKey.h"
#include "gradido_blockchain/data/compact/ConfirmedGradidoTx.h"
#include "gradido_blockchain/data/compact/PublicKeyIndex.h"

namespace gradido {
  using data::adapter::toPublicKey;
  using data::compact::PublicKeyIndex;

  namespace blockchain {
    CompactFilter::CompactFilter()
      : searchDirection(SearchDirection::DESC), transactionType(data::TransactionType::NONE), publicKeySearchType(PublicKeySearchType::None),
      coinCommunityIdIndex(0), maxTransactionNr(0), minTransactionNr(0),
      publicKeyIndex({})
    {

    }

    CompactFilter::CompactFilter(const Filter& filter, const IDictionary<PublicKey>& publicKeyDictionary, uint32_t communityIdIndex/* = 0*/)
      : searchDirection(filter.searchDirection), transactionType(filter.transactionType), publicKeySearchType(PublicKeySearchType::None),
      coinCommunityIdIndex(0), maxTransactionNr(filter.maxTransactionNr),
      minTransactionNr(filter.minTransactionNr),
      publicKeyIndex({}), pagination(filter.pagination), timepointInterval(filter.timepointInterval)
    {
      uint32_t index = 0;
      bool hasPublicKey = false;
      if (filter.updatedBalancePublicKey && !filter.updatedBalancePublicKey->isEmpty()) {
        index = publicKeyDictionary.getIndexForData(toPublicKey(filter.updatedBalancePublicKey));
        publicKeySearchType = PublicKeySearchType::BalanceChangingPublicKey;
        hasPublicKey = true;
      }
      else if (filter.involvedPublicKey && !filter.involvedPublicKey->isEmpty()) 
      {
        if (PublicKeySearchType::None != publicKeySearchType) {
          throw GradidoNodeInvalidDataException("couldn't handle involvedPublicKey and updatedBalancePublicKey in filter");
        }
        index = publicKeyDictionary.getIndexForData(toPublicKey(filter.involvedPublicKey));
        publicKeySearchType = PublicKeySearchType::InvolvedPublicKey;
        hasPublicKey = true;
      }
      else {
        publicKeySearchType = PublicKeySearchType::None;
      }
      if (hasPublicKey)
      {
        if (index && static_cast<uint32_t>(index) == index) {
          if (communityIdIndex) {
            publicKeyIndex = {
              .communityIdIndex = communityIdIndex,
              .publicKeyIndex = index
            };
          }
          else {
            publicKeySearchType = PublicKeySearchType::MissingCommunityId;
          }
        }
        else {
          publicKeySearchType = PublicKeySearchType::MissingIndex;
        }
      }
      if (filter.coinCommunityIdIndex) {
        coinCommunityIdIndex = *filter.coinCommunityIdIndex;
      }
    }

    CompactFilter CompactFilter::lastTransaction()
    {
      CompactFilter f;
      f.searchDirection = SearchDirection::DESC;
      f.pagination.size = 1;
      return f;
    };
    CompactFilter CompactFilter::firstTransaction()
    {
      CompactFilter f;
      f.searchDirection = SearchDirection::ASC;
      f.pagination.size = 1;
      return f;
    }

    CompactFilter CompactFilter::allTransactions()
    {
      CompactFilter f;
      f.pagination = Pagination(0, 0);
      return f;
    }

    CompactFilter CompactFilter::lastBalanceFor(PublicKeyIndex publicKeyIndex)
    {
      CompactFilter f;
      f.publicKeyIndex = publicKeyIndex;
      f.publicKeySearchType = PublicKeySearchType::BalanceChangingPublicKey;
      f.pagination.size = 1;
      f.searchDirection = SearchDirection::DESC;
      return f;
    }

    FilterResult CompactFilter::matches(const data::compact::ConfirmedGradidoTx& confirmedTx, FilterCriteria type) const
    {
			if ((type & FilterCriteria::TRANSACTION_NR) == FilterCriteria::TRANSACTION_NR)
			{
				if (minTransactionNr && confirmedTx.txNr < minTransactionNr) {
					return FilterResult::DISMISS;
				}
				if (maxTransactionNr && confirmedTx.txNr > maxTransactionNr) {
					return FilterResult::DISMISS;
				}
			}
			if ((type & FilterCriteria::COIN_COMMUNITY) == FilterCriteria::COIN_COMMUNITY && coinCommunityIdIndex)
			{
				if (!confirmedTx.hasCoinsFromCommunity(coinCommunityIdIndex)) {
					return FilterResult::DISMISS;
				}
			}
			if ((type & FilterCriteria::TRANSACTION_TYPE) == FilterCriteria::TRANSACTION_TYPE) {
				if (transactionType != data::TransactionType::NONE && confirmedTx.transactionType != transactionType) {
					return FilterResult::DISMISS;
				}
			}

			if ((type & FilterCriteria::TIMEPOINT_INTERVAL) == FilterCriteria::TIMEPOINT_INTERVAL)
			{
				if (!timepointInterval.isEmpty()) {
					if (!timepointInterval.isInsideInterval(confirmedTx.getConfirmedAt().getAsTimepoint())) {
						return FilterResult::DISMISS;
					}
				}
			}
			
			if ((type & FilterCriteria::INVOLVED_PUBLIC_KEY) == FilterCriteria::INVOLVED_PUBLIC_KEY)
			{
        if (PublicKeySearchType::InvolvedPublicKey == publicKeySearchType && !confirmedTx.isInvolved(publicKeyIndex)) {
          return FilterResult::DISMISS;
        }
			}
			if ((type & FilterCriteria::UPDATED_BALANCED_PUBLIC_KEY) == FilterCriteria::UPDATED_BALANCED_PUBLIC_KEY)
			{
        if (PublicKeySearchType::BalanceChangingPublicKey == publicKeySearchType && !confirmedTx.isBalanceUpdated(publicKeyIndex)) {
					return FilterResult::DISMISS;
				}
			}
			return FilterResult::USE;
    }
  }
}