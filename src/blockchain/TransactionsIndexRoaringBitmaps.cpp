#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/blockchain/PublicKeySearchType.h"
#include "gradido_blockchain/blockchain/RangeUtils.h"
#include "gradido_blockchain/blockchain/SearchDirection.h"
#include "gradido_blockchain/blockchain/TransactionsIndex.h"
#include "gradido_blockchain/blockchain/TransactionsIndexRoaringBitmaps.h"
#include "gradido_blockchain/data/AddressType.h"
#include "gradido_blockchain/data/compact/ConfirmedGradidoTx.h"
#include "gradido_blockchain/data/compact/PublicKeyIndex.h"
#include "gradido_blockchain/data/TransactionType.h"
#include "gradido_blockchain/memory/Block.h"

#include <date/date.h>
#include <loguru/loguru.hpp>
#include <magic_enum/magic_enum.hpp>
#include <roaring/roaring64Map.hh>

#include <algorithm>
#include <array>
#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace magic_enum;
using date::year_month_day;
using roaring::Roaring64Map;
using std::array, std::pair, std::unordered_map, std::nullopt, std::optional, std::vector;

namespace gradido {
  using data::compact::ConfirmedGradidoTx, data::compact::PublicKeyIndex, data::compact::PublicKeyIndexHash, data::compact::PublicKeyIndexEqual;
  using data::AddressType, data::TransactionType;

  namespace blockchain {
    TransactionsIndexRoaringBitmaps::TransactionsIndexRoaringBitmaps(uint32_t communityIdIndex)
      : mMinTransactionNr(0), mMaxTransactionNr(0), mMinDate{}, mMaxDate{}, 
      mCommunityIdIndex(communityIdIndex), mAddressIndex(communityIdIndex)
    {
      mDayMonthYearTxRanges.resize(MAGIC_NUMBER_TRANSACTION_INDEX_ENTRIES_RESIZE_STEP_SIZE);
    }
    TransactionsIndexRoaringBitmaps::~TransactionsIndexRoaringBitmaps()
    {
      reset();
    }

    void TransactionsIndexRoaringBitmaps::reset()
    {
      mMinTransactionNr = 0;
      mMaxTransactionNr = 0;
      mMinDate = { date::year(0), date::month(0), date::day() };
      mMaxDate = { date::year(0), date::month(0), date::day() };
      mDayMonthYearTxRanges.resize(100);
      mDayMonthYearTxRanges.shrink_to_fit();
      for (auto& monthYearEntry : mDayMonthYearTxRanges) {
        memset(monthYearEntry.data(), 0, 31 * sizeof(uint64_t));
      }
      mInvolvedPublicKeys.clear();
      for (auto& roaring64 : mTxPerType) {
        roaring64.clear();
      }
      mOtherCoinCommunityTx.clear();

    }

    void TransactionsIndexRoaringBitmaps::addTransactionIndices(const ConfirmedGradidoTx& tx, const IDictionary<PublicKey>& publicKeyDict)
    {
      if (!tx.isConfirmedTx() || !tx.hasColdData()) {
        throw GradidoNodeInvalidDataException("unconfirmed tx or without cold data for addTransactionIndices call");
      }
      auto confirmedAtYMD = timepointAsYearMonthDay(tx.getConfirmedAt().getAsTimepoint());
      updateYearMonthDayTxRange(confirmedAtYMD, tx.txNr);

      auto involvedAddresses = tx.getInvolvedAddresses();
      for (const auto& involvedAddress : involvedAddresses) {
        auto it = mInvolvedPublicKeys.insert({ involvedAddress, PublicKeyBitmaps{} }).first;
        if (tx.isSignaturePublicKey(involvedAddress, publicKeyDict)) {
          it->second.signedTx.add(tx.txNr);
        }
        if (tx.isBalanceUpdated(involvedAddress)) {
          it->second.balanceChangingTx.add(tx.txNr);
        }
        if (tx.isOtherInvolved(involvedAddress)) {
          if (!it->second.otherInvolvedTx) {
            it->second.otherInvolvedTx = std::make_unique<Roaring64Map>();
          }
          it->second.otherInvolvedTx->add(tx.txNr);
        }
      }
      auto& txTypeRoaring = mTxPerType[(unsigned)tx.transactionType];
      txTypeRoaring.add(tx.txNr);

      for (int i = 0; i < tx.accountBalanceCount; ++i) 
      {
        const auto& accountBalance = tx.accountBalances[i];
        if (accountBalance.coinCommunityIdIndex != tx.txCommunityIdIndex) {
          mOtherCoinCommunityTx.insert({ accountBalance.coinCommunityIdIndex, {} }).first->second.add(tx.txNr);
        }
      }
      mAddressIndex.addTransaction(tx);
    }

    vector<uint64_t> TransactionsIndexRoaringBitmaps::findTransactions(const CompactFilter& filter) const
    {
      if (!mMinTransactionNr && !mMaxTransactionNr) {
        return {};
      }
      auto resultSet = aggregateTransactions(filter);
      if (!resultSet) { return {}; }
      auto resultSetSize = resultSet->cardinality();
      if (!resultSetSize || resultSetSize <= filter.pagination.skipEntriesCount()) {
        return {};
      }
      if (filter.pagination.size) {
        uint64_t pageStartTx = 0;
        uint64_t pageEndTx = 0;
        // cut out range described by pagination
        if (SearchDirection::DESC == filter.searchDirection)
        {
          resultSet->select(resultSetSize - filter.pagination.skipEntriesCount() - 1, &pageEndTx);
          if (filter.pagination.size == 1) {
            return { pageEndTx };
          }
          if (resultSetSize <= filter.pagination.skipEntriesCount() + filter.pagination.size) {
            pageStartTx = resultSet->minimum();
          }
          else {
            resultSet->select(
              resultSetSize - filter.pagination.skipEntriesCount() - filter.pagination.size - 1,
              &pageStartTx
            );
          }
          if (filter.pagination.size == 2) {
            return { pageEndTx, pageStartTx };
          }
        }
        else if (SearchDirection::ASC == filter.searchDirection)
        {
          resultSet->select(filter.pagination.skipEntriesCount(), &pageStartTx);
          if (filter.pagination.size == 1) {
            return { pageStartTx };
          }
          if (!resultSet->select(filter.pagination.skipEntriesCount() + filter.pagination.size, &pageEndTx)) {
            pageEndTx = resultSet->maximum();
          }
          if (filter.pagination.size == 2) {
            return { pageStartTx, pageEndTx };
          }
        }
        else {
          throw GradidoUnhandledEnum("find with unknown", "SearchDirection", enum_name(filter.searchDirection).data());
        }
        if (pageEndTx < pageStartTx) {
          throw GradidoNodeInvalidDataException(
            "something went wrong, pageEndTx in TransactionIndex::findTransactions are smaller than pageStartTx"
          );
        }
        Roaring64Map paginationMap;
        paginationMap.addRangeClosed(pageStartTx, pageEndTx);
        *resultSet &= paginationMap;
      }
      vector<uint64_t> resultTxs;
      resultTxs.resize(resultSet->cardinality());
      resultSet->toUint64Array(reinterpret_cast<uint64_t*>(resultTxs.data()));
      if (SearchDirection::DESC == filter.searchDirection) {
        std::reverse(resultTxs.begin(), resultTxs.end());
      }
      return resultTxs;
    } 

    size_t TransactionsIndexRoaringBitmaps::countTransactions(const CompactFilter& filter) const
    {
      if (!mMinTransactionNr && !mMaxTransactionNr) {
        return 0;
      }
      auto result = aggregateTransactions(filter);
      if (!result) {
        return 0;
      }
      return result->cardinality();
    }

    StateChange<AddressType> TransactionsIndexRoaringBitmaps::getAddressType(PublicKeyIndex publicKeyIndex) const
    {
      auto addressType = mAddressIndex.getAddressType(publicKeyIndex);
      if (AddressType::NONE == addressType) {
        return AddressType::NONE;
      }
      auto txs = mAddressIndex.getAddressTypeChangingTransactions(publicKeyIndex);
      if (txs.empty()) {
        return addressType;
      }
      if (txs.size() == 1) {
        return { txs[0], addressType };
      }
      std::sort(txs.begin(), txs.end());
      return { txs.back(), mAddressIndex.getAddressType(publicKeyIndex) };
    }

    size_t TransactionsIndexRoaringBitmaps::yearMonthToIndex(year_month_day ymd) const
    {
      if (mMinDate.month() == date::month(0) && mMinDate.year() == date::year(0) && mMinDate.day() == date::day(0)) {
        LOG_F(WARNING, "yearMonthDayToIndex called while minDate is 0");
        return 0;
      }
      
      if (ymd < mMinDate) {
        return 0;
      }
      if (ymd > mMaxDate) {
        ymd = mMaxDate;
      }
      int years = int(ymd.year()) - int(mMinDate.year());
      int months = int(unsigned(ymd.month())) - int(unsigned(mMinDate.month()));
      return static_cast<size_t>(years * 12 + months);
    }

    pair<uint64_t, uint64_t> TransactionsIndexRoaringBitmaps::getTxRangeOfInterval(
      date::year_month_day startDate, date::year_month_day endDate
    ) const
    {
      uint64_t startTx = mMinTransactionNr;

      auto endIndex = yearMonthToIndex(endDate);

      auto ymIndexCursor = endIndex;
      auto dayIndexCursor = (unsigned)endDate.day() - 1;
      if (endDate > mMaxDate) {
        dayIndexCursor = 30;
      }
      auto startIndex = yearMonthToIndex(startDate);
      auto startDayIndex = (unsigned)startDate.day() - 1;

      while (!mDayMonthYearTxRanges[ymIndexCursor][dayIndexCursor]) {
        if (!dayIndexCursor && !ymIndexCursor) {
          return { 0, 0 };
        }
        if (ymIndexCursor == startIndex && startDayIndex == dayIndexCursor) {
          return { 0, 0 };
        }
        if (!dayIndexCursor) {
          --ymIndexCursor;
          dayIndexCursor = 30;
        }
        else {
          --dayIndexCursor;
        }
      }
      uint64_t endTx = mDayMonthYearTxRanges[ymIndexCursor][dayIndexCursor];

      if (startDate > mMinDate) {
        ymIndexCursor = startIndex;
        dayIndexCursor = startDayIndex;
        assert(ymIndexCursor || dayIndexCursor);        
        do {
          if (!dayIndexCursor && !ymIndexCursor) {
            break;
          }
          if (!dayIndexCursor) {
            --ymIndexCursor;
            dayIndexCursor = 30;
          }
          else {
            --dayIndexCursor;
          }
        } while (!mDayMonthYearTxRanges[ymIndexCursor][dayIndexCursor]);
        startTx = mDayMonthYearTxRanges[ymIndexCursor][dayIndexCursor] + 1;
      }
      assert(startTx <= endTx);
      return { startTx, endTx};
    }

    optional<pair<uint64_t, uint64_t>> TransactionsIndexRoaringBitmaps::calculateTxRange(const CompactFilter& filter) const
    {
      if (filter.minTransactionNr && filter.minTransactionNr > mMaxTransactionNr ||
        filter.maxTransactionNr && filter.maxTransactionNr < mMinTransactionNr) {
        return nullopt;
      }
      auto startDate = mMinDate;
      auto endDate = mMaxDate;
      if (!filter.timepointInterval.isEmpty()) {
        startDate = timepointAsYearMonthDay(filter.timepointInterval.getStartDate());
        endDate = timepointAsYearMonthDay(filter.timepointInterval.getEndDate());
      }
      if (startDate > mMaxDate || endDate < mMinDate) {
        return nullopt;
      }
      pair<uint64_t, uint64_t> txRangeInterval{ mMinTransactionNr, mMaxTransactionNr };
      if (startDate != mMinDate || endDate != mMaxDate) {
        txRangeInterval = getTxRangeOfInterval(startDate, endDate);
        if (!txRangeInterval.first && !txRangeInterval.second) {
          LOG_F(WARNING, "getTxRangeOfInterval return empty pair");
          return nullopt;
        }
      }
      if (filter.minTransactionNr && filter.minTransactionNr > txRangeInterval.first) {
        txRangeInterval.first = filter.minTransactionNr;
      }
      if (filter.maxTransactionNr && filter.maxTransactionNr < txRangeInterval.second) {
        txRangeInterval.second = filter.maxTransactionNr;
      }
      return txRangeInterval;
    }

    size_t TransactionsIndexRoaringBitmaps::yearMonthToIndexUpdateBounds(date::year year, date::month month, date::day day)
    {
      assert(mDayMonthYearTxRanges.size());
      auto currentMinDate = mMinDate;
      if (mMinDate.month() == date::month(0) && mMinDate.year() == date::year(0) && mMinDate.day() == date::day(0)) {
        mMinDate = date::year_month_day{ year, month, day };
      }

      int years = int(year) - int(mMinDate.year());
      int months = int(unsigned(month)) - int(unsigned(mMinDate.month()));
      auto index = static_cast<size_t>(years * 12 + months);

      if (index >= mDayMonthYearTxRanges.size()) {
        mDayMonthYearTxRanges.resize(mDayMonthYearTxRanges.size() + MAGIC_NUMBER_TRANSACTION_INDEX_ENTRIES_RESIZE_STEP_SIZE);
        if (index >= mDayMonthYearTxRanges.size()) {
          mMinDate = currentMinDate;
          throw GradidoNodeInvalidDataException("unexpected jump in transaction index grow");
        }
      }

      auto newMaxYearMonth = date::year_month_day{ year, month, day };
      if (newMaxYearMonth > mMaxDate) {
        mMaxDate = newMaxYearMonth;
      }

      return index;
    }

    void TransactionsIndexRoaringBitmaps::updateYearMonthDayTxRange(date::year_month_day date, uint64_t txNr)
    {
      if (!mMinTransactionNr) {
        mMinTransactionNr = txNr;
      }
      if (txNr < mMinTransactionNr) {
        throw GradidoNodeInvalidDataException("call updateYearMonthDayTxRange with txNr < mMinTransactionNr");
      }
      if (txNr > mMaxTransactionNr) {
        mMaxTransactionNr = txNr;
      }
      auto ymIndex = yearMonthToIndexUpdateBounds(date.year(), date.month(), date.day());
      auto dayIndex = (unsigned)date.day() - 1;
      auto& maxTxNr = mDayMonthYearTxRanges[ymIndex][dayIndex];
      if (txNr > maxTxNr) {
        maxTxNr = txNr;
      }
    }

    optional<Roaring64Map> TransactionsIndexRoaringBitmaps::aggregateTransactions(const CompactFilter& filter) const
    {
      array<const Roaring64Map*, 5> roaringMaps = { nullptr };
      uint8_t roaringMapLastCursor = 0;
      thread_local static Roaring64Map involvedPublicKeys;
      thread_local static Roaring64Map rangeMap;
      const Roaring64Map* coinCommunityRevertMap = nullptr;

      auto txRangeIntervalOptional = calculateTxRange(filter);
      if (!txRangeIntervalOptional) {
        return nullopt;
      }

      if (filter.coinCommunityIdIndex) {
        auto it = mOtherCoinCommunityTx.find(filter.coinCommunityIdIndex);
        // if user search for tx with foreign gdd balances
        if (filter.coinCommunityIdIndex != mCommunityIdIndex) {
          if (it != mOtherCoinCommunityTx.end()) {
            roaringMaps[roaringMapLastCursor++] = &it->second;
          }
          else {
            return nullopt;
          }
        }
        // if user will only results with gdd balances from this community
        else {
          if (it != mOtherCoinCommunityTx.end()) {
            coinCommunityRevertMap = &it->second;
          }
        }
      }
      
      if (PublicKeySearchType::BalanceChangingPublicKey == filter.publicKeySearchType)
      {
        if (filter.publicKeyIndex.communityIdIndex != mCommunityIdIndex) {
          LOG_F(WARNING, "called findTransactions with BalanceChangingPublicKey from another community");
          return nullopt;
        }
        else {
          auto it = mInvolvedPublicKeys.find(filter.publicKeyIndex);
          if (it != mInvolvedPublicKeys.end()) {
            roaringMaps[roaringMapLastCursor++] = &it->second.balanceChangingTx;
          }
          else {
            return nullopt;
          }
        }
      }
      else if (PublicKeySearchType::InvolvedPublicKey == filter.publicKeySearchType) {        
        auto it = mInvolvedPublicKeys.find(filter.publicKeyIndex);
        if (it == mInvolvedPublicKeys.end()) {
          return nullopt;
        }
        involvedPublicKeys.clear();
        if (it->second.otherInvolvedTx) {
          involvedPublicKeys |= *it->second.otherInvolvedTx;
        }
        involvedPublicKeys |= it->second.signedTx;
        involvedPublicKeys |= it->second.balanceChangingTx;

        if (involvedPublicKeys.isEmpty()) {
          return nullopt;
        }
        else {
          roaringMaps[roaringMapLastCursor++] = &involvedPublicKeys;
        }
      }
      
      if (TransactionType::NONE != filter.transactionType) {
        if (!mTxPerType[(unsigned)filter.transactionType].isEmpty()) {
          roaringMaps[roaringMapLastCursor++] = &mTxPerType[(unsigned)filter.transactionType];
        }
        else {
          return nullopt;
        }
      }
      
      if (
        txRangeIntervalOptional->first != mMinTransactionNr ||
        txRangeIntervalOptional->second != mMaxTransactionNr ||
        !roaringMapLastCursor
        ) {
        if (rangeMap.minimum() != txRangeIntervalOptional->first || rangeMap.maximum() != txRangeIntervalOptional->second) {
          rangeMap.clear();
          rangeMap.addRangeClosed(txRangeIntervalOptional->first, txRangeIntervalOptional->second);
        }
        roaringMaps[roaringMapLastCursor++] = &rangeMap;
      }

      if (roaringMapLastCursor > 1) {
        // sort maps to calculate from smallest on, should be faster say the docs
        std::sort(roaringMaps.begin(), roaringMaps.begin() + roaringMapLastCursor,
          [](const Roaring64Map* a, const Roaring64Map* b) -> bool
          {
            return a->cardinality() < b->cardinality();
          }
        );
        Roaring64Map resultSet;
        resultSet = *roaringMaps[0] & *roaringMaps[1];
        if (roaringMapLastCursor > 2) {
          for (int i = 2; i < roaringMapLastCursor; ++i) {
            resultSet &= *roaringMaps[i];
          }
        }
        if (coinCommunityRevertMap) {
          resultSet -= *coinCommunityRevertMap;
        }
        if (resultSet.isEmpty()) {
          return nullopt;
        }
        return resultSet;
      }  
      if (coinCommunityRevertMap) {
        return *roaringMaps[0] - *coinCommunityRevertMap;
      }
      if (roaringMaps[0]->isEmpty()) {
        return nullopt;
      }
      return *roaringMaps[0];
    }
  }
}

