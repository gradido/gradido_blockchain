#include "gradido_blockchain/blockchain/CompactFilter.h"
#include "gradido_blockchain/data/adapter/publicKey.h"
#include "gradido_blockchain/data/compact/ConfirmedGradidoTx.h"
#include "gradido_blockchain/data/compact/PublicKeyIndex.h"

namespace gradido {
  using data::adapter::toPublicKey;
  using data::compact::PublicKeyIndex;

  namespace blockchain {
    CompactFilter::CompactFilter()
      : minTransactionNr(0), maxTransactionNr(0),
      publicKeyIndex(0), coinCommunityIdIndex(0),
      transactionType(data::TransactionType::NONE), searchDirection(SearchDirection::DESC), publicKeySearchType(PublicKeySearchType::None),
      communityIdType(CommunityIdType::NONE)
    {
      
    }

    CompactFilter::CompactFilter(const Filter& filter, const IDictionary<PublicKey>& publicKeyDictionary)
      : minTransactionNr(filter.minTransactionNr), maxTransactionNr(filter.maxTransactionNr),
      publicKeyIndex(0), coinCommunityIdIndex(0),
      transactionType(filter.transactionType), searchDirection(filter.searchDirection), publicKeySearchType(PublicKeySearchType::None),
      communityIdType(CommunityIdType::NONE), pagination(filter.pagination), timepointInterval(filter.timepointInterval)
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
        communityIdType = CommunityIdType::COIN_COMMUNITY_ID;
      }
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
			if ((type & FilterCriteria::COIN_COMMUNITY) == FilterCriteria::COIN_COMMUNITY && 
          (communityIdType & CommunityIdType::COIN_COMMUNITY_ID) == CommunityIdType::COIN_COMMUNITY_ID)
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
        PublicKeyIndex pubIdx{ .communityIdIndex = confirmedTx.txCommunityIdIndex, .publicKeyIndex = publicKeyIndex };
        if ((communityIdType & CommunityIdType::PUBLIC_KEY_COMMUNITY_ID) == CommunityIdType::PUBLIC_KEY_COMMUNITY_ID) {
          pubIdx.communityIdIndex = coinCommunityIdIndex;
        }
        if (PublicKeySearchType::InvolvedPublicKey == publicKeySearchType && !confirmedTx.isInvolved(pubIdx)) {
          return FilterResult::DISMISS;
        }
			}
			if ((type & FilterCriteria::UPDATED_BALANCED_PUBLIC_KEY) == FilterCriteria::UPDATED_BALANCED_PUBLIC_KEY)
			{
        PublicKeyIndex pubIdx{ .communityIdIndex = confirmedTx.txCommunityIdIndex, .publicKeyIndex = publicKeyIndex };
        if ((communityIdType & CommunityIdType::PUBLIC_KEY_COMMUNITY_ID) == CommunityIdType::PUBLIC_KEY_COMMUNITY_ID) {
          pubIdx.communityIdIndex = coinCommunityIdIndex;
        }
        if (PublicKeySearchType::BalanceChangingPublicKey == publicKeySearchType && !confirmedTx.isBalanceUpdated(pubIdx)) {
					return FilterResult::DISMISS;
				}
			}
			return FilterResult::USE;
    }
  }
}