#ifndef GRADIDO_BLOCKCHAIN_BLOCKCHAIN_TRANSACTIONS_INDEX_ROARING_BITMAPS_H
#define GRADIDO_BLOCKCHAIN_BLOCKCHAIN_TRANSACTIONS_INDEX_ROARING_BITMAPS_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/blockchain/AddressIndex.h"
#include "gradido_blockchain/blockchain/FilterResult.h"
#include "gradido_blockchain/blockchain/StateChange.h"
#include "gradido_blockchain/data/TransactionType.h"
#include "gradido_blockchain/data/compact/PublicKeyIndex.h"
#include "gradido_blockchain/lib/DictionaryInterface.h"
#include "gradido_blockchain/lib/TimepointInterval.h"

#include <date/date.h>
#include <roaring/roaring64map.hh>

#include <array>
#include <deque>
#include <functional>
#include <optional>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

namespace gradido {
  namespace data {
    namespace compact {
      struct ConfirmedGradidoTx;
    }
  }
  namespace blockchain {

    class GRADIDOBLOCKCHAIN_EXPORT TransactionsIndexRoaringBitmaps 
    {
    public:
      TransactionsIndexRoaringBitmaps(uint32_t communityIdIndex);
      ~TransactionsIndexRoaringBitmaps();

      void reset();

      void addTransactionIndices(const data::compact::ConfirmedGradidoTx& tx, const IDictionary<PublicKey>& publicKeyDict);
      std::vector<uint64_t> findTransactions(const CompactFilter& filter) const;
      size_t countTransactions(const CompactFilter& filter) const;
      StateChange<data::AddressType> getAddressType(data::compact::PublicKeyIndex publicKeyIndex) const;

      size_t yearMonthToIndex(date::year_month_day ymd) const;
      std::pair<uint64_t, uint64_t> getTxRangeOfInterval(date::year_month_day startDate, date::year_month_day endDate) const;
      //! calculate range interval from filter min + max tx nr, timepoint interval and local min + max tx nr and date range
      //! \return pair of minTxNr and maxTxNr 
      std::optional<std::pair<uint64_t, uint64_t>> calculateTxRange(const CompactFilter& filter) const;

      inline uint64_t getMinTransactionNr() const { return mMinTransactionNr; }
      inline uint64_t getMaxTransactionNr() const { return mMaxTransactionNr; }
      
    protected:
      //! return index for mDayMonthYearTxRanges
      size_t yearMonthToIndexUpdateBounds(date::year year, date::month month, date::day day);
      void updateYearMonthDayTxRange(date::year_month_day date, uint64_t txNr);

      //! \return nullopt if tx not exist which can fullfill all criteria, return maybe empty roaring if filter was empty
      std::optional<roaring::Roaring64Map> aggregateTransactions(const CompactFilter& filter) const;

      uint64_t				     mMinTransactionNr;
      uint64_t				     mMaxTransactionNr;
      date::year_month_day mMinDate;
      date::year_month_day mMaxDate;
      uint32_t             mCommunityIdIndex;

      // is used like a cache, even from const
      mutable AddressIndex mAddressIndex;

      // store max tx nr per Day
      std::vector<std::array<uint64_t, 31>> mDayMonthYearTxRanges;
      struct PublicKeyBitmaps
      {
        roaring::Roaring64Map balanceChangingTx;
        roaring::Roaring64Map signedTx;
        std::shared_ptr<roaring::Roaring64Map> otherInvolvedTx;
      };
      
      std::unordered_map<
        data::compact::PublicKeyIndex, 
        PublicKeyBitmaps,
        data::compact::PublicKeyIndexHash,
        data::compact::PublicKeyIndexEqual
      > mInvolvedPublicKeys;

      // store txs per TransactionType
      std::array<roaring::Roaring64Map, (size_t)data::TransactionType::MAX_VALUE> mTxPerType;
      // store txs with other coin community id as the blockchain in which the txs are 
      std::unordered_map<uint32_t, roaring::Roaring64Map> mOtherCoinCommunityTx;
    };
  }
}

#endif // GRADIDO_BLOCKCHAIN_BLOCKCHAIN_TRANSACTIONS_INDEX_ROARING_BITMAPS_H
