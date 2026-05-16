#include "gradido_blockchain_core/data/wire/confirmed_transaction.h"
#include "gradido_blockchain_core/data/wire/transaction_body.h"
#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/data/adapter/types.h"
#include "gradido_blockchain/data/adapter/timestamp.h"
#include "gradido_blockchain/data/compact/AccountBalance.h"
#include "gradido_blockchain/data/compact/ConfirmedGradidoTx.h"
#include "gradido_blockchain/data/compact/ConfirmedGradidoTxCold.h"
#include "gradido_blockchain/data/compact/PublicKeyIndex.h"
#include "gradido_blockchain/data/rich/AccountBalance.h"
#include "gradido_blockchain/lib/DictionaryExceptions.h"
#include "gradido_blockchain/lib/Uuid.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

#include "loguru/loguru.hpp"
#include "magic_enum/magic_enum.hpp"

#include <algorithm>
#include <array>
#include <memory>
#include <vector>

using namespace magic_enum;
using std::string, std::to_string;
using std::make_unique, std::unique_ptr;
using std::array, std::sort, std::unique, std::vector;

namespace gradido::data::compact {

  ConfirmedGradidoTx::ConfirmedGradidoTx()
    : txNr(0), confirmedAtSeconds(0), confirmedAtNanos(0), txCommunityIdIndex(0), coinCommunityIdIndex(0),
    crossGroupType(CrossGroupType::LOCAL), transactionType(TransactionType::NONE), balanceDerivationType(BalanceDerivationType::UNSPECIFIED),
    accountBalanceCount(0),
    coldData(nullptr)
  {
  }

  ConfirmedGradidoTx::~ConfirmedGradidoTx()
  {
  }

  // copy
  ConfirmedGradidoTx::ConfirmedGradidoTx(const ConfirmedGradidoTx& other)
    : txNr(other.txNr),
    confirmedAtSeconds(other.confirmedAtSeconds),
    confirmedAtNanos(other.confirmedAtNanos),
    txCommunityIdIndex(other.txCommunityIdIndex),
    coinCommunityIdIndex(other.coinCommunityIdIndex),
    crossGroupType(other.crossGroupType),
    transactionType(other.transactionType),
    balanceDerivationType(other.balanceDerivationType),
    accountBalanceCount(other.accountBalanceCount),
    coldData(nullptr)
  {
    memcpy(accountBalances, other.accountBalances, sizeof(AccountBalance) * other.accountBalanceCount);
    memcpy(&specific, &other.specific, sizeof(specific));

    // deep copy cold data
    if (other.coldData) {
      coldData = make_unique<ConfirmedGradidoTxCold>(*other.coldData);
    }
  }

  // move
  ConfirmedGradidoTx::ConfirmedGradidoTx(ConfirmedGradidoTx&& other) noexcept
    : txNr(other.txNr),
    confirmedAtSeconds(other.confirmedAtSeconds),
    confirmedAtNanos(other.confirmedAtNanos),
    txCommunityIdIndex(other.txCommunityIdIndex),
    coinCommunityIdIndex(other.coinCommunityIdIndex),
    crossGroupType(other.crossGroupType),
    transactionType(other.transactionType),
    balanceDerivationType(other.balanceDerivationType),
    accountBalanceCount(other.accountBalanceCount),
    coldData(std::move(other.coldData))
  {
    memcpy(accountBalances, other.accountBalances, sizeof(AccountBalance) * other.accountBalanceCount);
    memcpy(&specific, &other.specific, sizeof(specific));

    // neuter the source
    other.accountBalanceCount = 0;
    other.transactionType = TransactionType::NONE;
  }
  // move
  ConfirmedGradidoTx& ConfirmedGradidoTx::operator=(ConfirmedGradidoTx&& other) noexcept
  {
    txNr = other.txNr; 
    confirmedAtSeconds = other.confirmedAtSeconds;
    confirmedAtNanos = other.confirmedAtNanos;
    txCommunityIdIndex = other.txCommunityIdIndex;
    coinCommunityIdIndex = other.coinCommunityIdIndex;
    crossGroupType = other.crossGroupType;
    transactionType = other.transactionType;
    balanceDerivationType = other.balanceDerivationType;
    accountBalanceCount = other.accountBalanceCount;
    coldData = std::move(other.coldData);

    memcpy(accountBalances, other.accountBalances, sizeof(AccountBalance) * other.accountBalanceCount);
    memcpy(&specific, &other.specific, sizeof(specific));

    // neuter the source
    other.accountBalanceCount = 0;
    other.transactionType = TransactionType::NONE;
    return *this;
  }

  // copy
  ConfirmedGradidoTx& ConfirmedGradidoTx::operator=(const ConfirmedGradidoTx& other)
  {
    txNr = other.txNr; 
    confirmedAtSeconds = other.confirmedAtSeconds;
    confirmedAtNanos = other.confirmedAtNanos;
    txCommunityIdIndex = other.txCommunityIdIndex;
    coinCommunityIdIndex = other.coinCommunityIdIndex;
    crossGroupType = other.crossGroupType;
    transactionType = other.transactionType;
    balanceDerivationType = other.balanceDerivationType;
    accountBalanceCount = other.accountBalanceCount;
    
    memcpy(accountBalances, other.accountBalances, sizeof(AccountBalance) * other.accountBalanceCount);
    memcpy(&specific, &other.specific, sizeof(specific));

    // deep copy cold data
    if (other.coldData) {
      coldData = make_unique<ConfirmedGradidoTxCold>(*other.coldData);
    }

    return *this;
  }

  ConfirmedGradidoTx ConfirmedGradidoTx::fromGrdw(
    const grdw_confirmed_transaction* tx,
    uint32_t blockchainCommunityIdIndex,
    AppContext& appContext,
    bool loadColdData/* = true*/
  ) {
    ConfirmedGradidoTx confirmedTx;
    confirmedTx.txNr = tx->id;
    confirmedTx.confirmedAtSeconds = tx->confirmed_at.seconds;
    confirmedTx.confirmedAtNanos = tx->confirmed_at.nanos;
    confirmedTx.txCommunityIdIndex = blockchainCommunityIdIndex;
    confirmedTx.coinCommunityIdIndex = blockchainCommunityIdIndex;
    confirmedTx.balanceDerivationType = adapter::fromGrdw(tx->balance_derivation);
    if (tx->account_balances_count > 3) {
      throw GradidoNotImplementedException("more than 3 account balances, currently not supported from compact::ConfirmedGradidoTx");
    }
    confirmedTx.accountBalanceCount = tx->account_balances_count;
    for (int i = 0; i < tx->account_balances_count; i++)
    {
      auto& accountBalance = confirmedTx.accountBalances[i];
      const auto& tx_account_balance = tx->account_balances[i];
      accountBalance.balanceGddCent = tx->account_balances[i].balance;
      accountBalance.coinCommunityIdIndex = blockchainCommunityIdIndex;
      
      Uuid communityUuid(tx_account_balance.community_uuid);
      accountBalance.coinCommunityIdIndex = appContext.getOrAddCommunityIdIndex(communityUuid.toString());
      
      accountBalance.publicKeyIndex = appContext.getOrAddPublicKeyIndex(blockchainCommunityIdIndex, tx_account_balance.pubkey);
    }
    
    if (loadColdData) {
      confirmedTx.coldData = ConfirmedGradidoTxCold::fromGrdw(tx);
    }
    return confirmedTx;
  }

  ConfirmedGradidoTx ConfirmedGradidoTx::fromGrdw(
    const grdw_confirmed_transaction* tx,
    const grdw_transaction_body* body,
    uint32_t blockchainCommunityIdIndex,
    AppContext& appContext,
    bool loadColdData/* = true */
  ) {
    auto confirmedTx = fromGrdw(tx, blockchainCommunityIdIndex, appContext, loadColdData);
    confirmedTx.fillFromGrdwTransactionBody(body, appContext);
    return confirmedTx;
  }

  ConfirmedGradidoTx ConfirmedGradidoTx::fromGrdw(
    const grdw_gradido_transaction* tx,
    uint32_t blockchainCommunityIdIndex,
    AppContext& appContext,
    bool loadColdData/* = true */
  ) {
    ConfirmedGradidoTx confirmedTx;
    confirmedTx.txNr = 0;
    confirmedTx.confirmedAtSeconds = 0;
    confirmedTx.confirmedAtNanos = 0;
    confirmedTx.txCommunityIdIndex = blockchainCommunityIdIndex;
    confirmedTx.coinCommunityIdIndex = blockchainCommunityIdIndex;
    confirmedTx.balanceDerivationType = BalanceDerivationType::UNSPECIFIED;
    
    if (loadColdData) {
      confirmedTx.coldData = ConfirmedGradidoTxCold::fromGrdw(tx);
    }
    return confirmedTx;
  }

  ConfirmedGradidoTx ConfirmedGradidoTx::fromGrdw(
    const grdw_gradido_transaction* tx,
    const grdw_transaction_body* body,
    uint32_t blockchainCommunityIdIndex,
    AppContext& appContext,
    bool loadColdData/* = true */
  ) {
    auto confirmedTx = fromGrdw(tx, blockchainCommunityIdIndex, appContext, loadColdData);
    confirmedTx.fillFromGrdwTransactionBody(body, appContext);
    return confirmedTx;
  }

  void ConfirmedGradidoTx::fillFromGrdwTransactionBody(const grdw_transaction_body* body, AppContext& appContext)
  {
    crossGroupType = adapter::fromGrdw(body->type);
    transactionType = adapter::fromGrdw(body->transaction_type);
    
    uint32_t senderCommunityIdIndex = txCommunityIdIndex;
    uint32_t recipientCommunityIdIndex = txCommunityIdIndex;
    if (crossGroupType != CrossGroupType::LOCAL) 
    {
      if (!body->other_community_uuid) {
        throw GradidoNodeInvalidDataException("missing other community uuid on cross group transaction");
      }
      Uuid otherUuid(body->other_community_uuid);
      switch (crossGroupType) {
      case CrossGroupType::OUTBOUND:
        recipientCommunityIdIndex = appContext.getOrAddCommunityIdIndex(otherUuid.toString());
        break;
      case CrossGroupType::INBOUND:
        senderCommunityIdIndex = appContext.getOrAddCommunityIdIndex(otherUuid.toString());
        break;
      default:
        throw GradidoUnhandledEnum(
          "compact confirmed transaction from grdw, not implemented yet",
          enum_type_name<decltype(crossGroupType)>().data(),
          enum_name(crossGroupType).data()
        );
      }
    }
    if (TransactionType::TIMEOUT_DEFERRED_TRANSFER == transactionType)
    {
      int senderIdx = -1;
      for (int i = 0; i < accountBalanceCount; i++) {
        if (!accountBalances[i].balanceGddCent) {
          if (senderIdx == -1) {
            senderIdx = i;
          }
          else {
            senderIdx = -2;
          }
        }
      }
      // we can determine the values from the account balances 
      if (accountBalanceCount == 2 && senderIdx > 0) {
        // with accountBalanceCount = 2 we have the indices 0 and 1, one of it is senderIdx, the other is !senderIdx
        specific.transfer.senderPublicKeyIndex = accountBalances[senderIdx].publicKeyIndex;
        specific.transfer.recipientPublicKeyIndex = accountBalances[!senderIdx].publicKeyIndex;
        // we cannot detect amount from this
        specific.transfer.amountGddCent = 0;
      }
      else {
        auto& blockchain = appContext.getCommunityContext(txCommunityIdIndex).getBlockchain();
        auto deferredTransfer = blockchain->getConfirmedTxForId(body->timeout_deferred_transfer.deferred_transfer_transaction_nr);
        if (!deferredTransfer) {
          throw GradidoNodeInvalidDataException("missing deferred transfer for timeout");
        }
        specific.transfer.senderPublicKeyIndex = deferredTransfer->specific.deferredTransfer.recipientPublicKeyIndex;
        specific.transfer.recipientPublicKeyIndex = deferredTransfer->specific.deferredTransfer.senderPublicKeyIndex;
        auto amount = GradidoUnit::fromGradidoCent(deferredTransfer->specific.deferredTransfer.amountGddCent);
        specific.transfer.amountGddCent = amount.calculateDecay(deferredTransfer->getConfirmedAt(), getConfirmedAt());
      }
    } 
    else 
    {
      switch (transactionType) {
      case TransactionType::TRANSFER:
        specific.transfer = TransferTx::fromGrdw(&body->transfer, senderCommunityIdIndex, recipientCommunityIdIndex, appContext);
        coinCommunityIdIndex = appContext.getOrAddCommunityIdIndex(Uuid(body->transfer.sender.community_uuid).toString());
        break;
      case TransactionType::CREATION:
        specific.creation = {
          .amountGddCent = body->creation.recipient.amount,
          .recipientPublicKeyIndex = appContext.getOrAddPublicKeyIndex(recipientCommunityIdIndex, body->creation.recipient.pubkey),
          .targetMonthYear = adapter::fromGrdw(body->creation.target_date).getAsYearMonth()
        };
        coinCommunityIdIndex = appContext.getOrAddCommunityIdIndex(Uuid(body->creation.recipient.community_uuid).toString());
        break;
      case TransactionType::REGISTER_ADDRESS:
        if (body->register_address.derivation_index != static_cast<uint16_t>(body->register_address.derivation_index)) {
          throw GradidoNodeInvalidDataException("derivation index exceed uint16_t");
        }
        specific.registerAddress = {
          .addressType = adapter::fromGrdw(body->register_address.address_type),
          .derivationIndex = static_cast<uint16_t>(body->register_address.derivation_index),
          .nameHashIndex = appContext.getOrAddUserNameHashIndex(body->register_address.name_hash),
          .userPublicKeyIndex = appContext.getOrAddPublicKeyIndex(senderCommunityIdIndex, body->register_address.user_pubkey),
          .accountPublicKeyIndex = appContext.getOrAddPublicKeyIndex(senderCommunityIdIndex, body->register_address.account_pubkey)
        };
        break;
      case TransactionType::DEFERRED_TRANSFER:
        if (crossGroupType != CrossGroupType::LOCAL) {
          throw GradidoNodeInvalidDataException("deferred transfer (currently) don't work cross community");
        }
        if (body->deferred_transfer.transfer.sender.amount != (int64_t)((uint32_t)(body->deferred_transfer.transfer.sender.amount))) {
          throw GradidoNodeInvalidDataException("deferred transfer amount exceed uint32_t (more than 429'496 GDD)");
        }
        specific.deferredTransfer = {
          .amountGddCent = (uint32_t)(body->deferred_transfer.transfer.sender.amount),
          .senderPublicKeyIndex = g_appContext->getOrAddPublicKeyIndex(txCommunityIdIndex, body->deferred_transfer.transfer.sender.pubkey),
          .recipientPublicKeyIndex = g_appContext->getOrAddPublicKeyIndex(txCommunityIdIndex, body->deferred_transfer.transfer.recipient),
          .timeoutDurationSeconds = body->deferred_transfer.timeout_duration
        };
        coinCommunityIdIndex = appContext.getOrAddCommunityIdIndex(Uuid(body->deferred_transfer.transfer.sender.community_uuid).toString());
        break;
      case TransactionType::REDEEM_DEFERRED_TRANSFER:
        specific.transfer = TransferTx::fromGrdw(&body->redeem_deferred_transfer.transfer, senderCommunityIdIndex, recipientCommunityIdIndex, appContext);
        coinCommunityIdIndex = appContext.getOrAddCommunityIdIndex(Uuid(body->redeem_deferred_transfer.transfer.sender.community_uuid).toString());
        break;
        //case TransactionType::TIMEOUT_DEFERRED_TRANSFER:
          // throw GradidoUnhandledEnum("on ConfirmedGradidoTx::fromGrdw, removed because of simplify", "TransactionType", enum_name(transactionType).data());

          //break;
      case TransactionType::COMMUNITY_ROOT:
        specific.communityRoot = {
          .publicKeyIndex = appContext.getOrAddPublicKeyIndex(senderCommunityIdIndex, body->community_root.pubkey),
          .gmwPublicKeyIndex = appContext.getOrAddPublicKeyIndex(senderCommunityIdIndex, body->community_root.gmw_pubkey),
          .aufPublicKeyIndex = appContext.getOrAddPublicKeyIndex(senderCommunityIdIndex, body->community_root.auf_pubkey)
        };
        break;
      default:
        throw GradidoUnhandledEnum("on ConfirmedGradidoTx::fromGrdw", "TransactionType", enum_name(transactionType).data());
      }
    }
    if (coldData) {
      coldData->fillFromGrdwTransactionBody(body, appContext);
    }
  }

  vector<PublicKeyIndex> ConfirmedGradidoTx::getInvolvedAddresses() const
  {
    constexpr size_t maxPublicKeysCount = 9;
    array<PublicKeyIndex, maxPublicKeysCount> result({});
    size_t resultCursor = 0;
 
    for (int i = 0; i < accountBalanceCount; i++) 
    {
      result[resultCursor++] = PublicKeyIndex{
        .communityIdIndex = txCommunityIdIndex,
        .publicKeyIndex = accountBalances[i].publicKeyIndex
      };
      if (resultCursor >= maxPublicKeysCount) {
        throw GradidoNodeInvalidDataException("more public key as expected in account balances");
      }
    }
    if (coldData) 
    {
      for (const auto& sigPair : coldData->signatureMap) 
      {
        result[resultCursor++] = PublicKeyIndex{
          .communityIdIndex = txCommunityIdIndex,
          // TODO: check if it make sense to store public key index in cold
          .publicKeyIndex = g_appContext->getOrAddPublicKeyIndex(txCommunityIdIndex, sigPair.first)
        };
        if (resultCursor >= maxPublicKeysCount) {
          throw GradidoNodeInvalidDataException("more public key as expected in signature map + account balances");
        }
      }
    }

    if (resultCursor + 3 >= maxPublicKeysCount) {
      throw GradidoNodeInvalidDataException("more public key as expected in signature map + account balances + body");
    }
    
    switch (transactionType) {
    case TransactionType::TRANSFER:
    case TransactionType::DEFERRED_TRANSFER:
    case TransactionType::REDEEM_DEFERRED_TRANSFER:
    case TransactionType::TIMEOUT_DEFERRED_TRANSFER:
      result[resultCursor++] = getSender();
      [[fallthrough]]; // getRecipient is also needed for transfer and deferred transfer
    case TransactionType::CREATION:
      result[resultCursor++] = getRecipient(); // used for creation, transfer and deferred transfer
      break;
    case TransactionType::REGISTER_ADDRESS:
      result[resultCursor++] = getRegisteredUser();
      result[resultCursor++] = getRegisteredAccount();
      break;
    //case TransactionType::TIMEOUT_DEFERRED_TRANSFER:
//      throw GradidoUnhandledEnum("on ConfirmedGradidoTx::getInvolvedAddresses, removed because of simplify", "TransactionType", enum_name(transactionType).data());
  //    break;
    case TransactionType::COMMUNITY_ROOT:
      result[resultCursor++] = getCommunityRootPublicKey();
      result[resultCursor++] = getAuf();
      result[resultCursor++] = getGmw();
      break;
    default: 
      throw GradidoUnhandledEnum("on ConfirmedGradidoTx::getInvolvedAddresses", "TransactionType", enum_name(transactionType).data());
    }
    sort(result.begin(), result.end());
    auto beginIt = result.begin();
    auto endIt = unique(result.begin(), result.end());
    while (beginIt->empty() && beginIt != endIt) {
      ++beginIt;
    }
    return vector<PublicKeyIndex>(beginIt, endIt);
  }

  vector<uint32_t> ConfirmedGradidoTx::getBalanceChangingPublicKeyIndices() const
  {
    vector<uint32_t> result(accountBalanceCount, 0);
    for (int i = 0; i < accountBalanceCount; ++i) {
      result[i] = accountBalances[i].publicKeyIndex;
    }
    return result;
  }

  std::vector<uint32_t> ConfirmedGradidoTx::getSignaturePublicKeyIndices(const IDictionary<PublicKey>& publicKeyDict) const
  {
    if (!coldData) return {};
    vector<uint32_t> result(coldData->signatureMap.size(), 0);
    for (int i = 0; i < coldData->signatureMap.size(); ++i) {
      result[i] = publicKeyDict.getIndexForData(coldData->signatureMap[i].first);
      if (!result[i]) {
        throw DictionaryMissingEntryException("missing index for signature public key", coldData->signatureMap[i].first.convertToHex());
      }
    }
    return result;
  }

  bool ConfirmedGradidoTx::isSignaturePublicKey(PublicKeyIndex pubkexIndex, const IDictionary<PublicKey>& publicKeyDict) const
  {
    if (pubkexIndex.communityIdIndex != txCommunityIdIndex) {
      return false;
    }
    if (!coldData) {
      throw GradidoNodeInvalidDataException("missing cold data for checking signature");
    }
    auto publicKey = publicKeyDict.getDataForIndexOrThrow(pubkexIndex.publicKeyIndex);
    for (const auto& publicKeySignaturePair: coldData->signatureMap) {
      if (publicKey.isTheSame(publicKeySignaturePair.first)) {
        return true;
      }
    }
    return false;
  }

  std::vector<PublicKeyIndex> ConfirmedGradidoTx::getOtherInvolved() const
  {
    if (!isCrossCommunityTx()) {
      if (isCommunityRoot()) {
        return { getCommunityRootPublicKey() };
      }
      return {};
    }
    if (isInboundCrossCommunityTx()) {
      return { getSender() };
    }
    else if (isOutboundCommunityTx()) {
      return { getRecipient() };
    }
    return {};
  }

  bool ConfirmedGradidoTx::isOtherInvolved(PublicKeyIndex pubkeyIndex) const
  {
    auto otherInvolved = getOtherInvolved();
    for (const auto& other : otherInvolved) {
      if (other == pubkeyIndex) {
        return true;
      }
    }
    return false;
  }

  bool ConfirmedGradidoTx::isBalanceUpdated(PublicKeyIndex pubkeyIndex) const
  {
    for (int i = 0; i < accountBalanceCount; i++) {
      if (accountBalances[i].publicKeyIndex == pubkeyIndex.publicKeyIndex && pubkeyIndex.communityIdIndex == txCommunityIdIndex) {
        return true;
      }
    }
    return false;
  }

  bool ConfirmedGradidoTx::isInvolved(PublicKeyIndex pubkeyIndex) const
  {
    if (pubkeyIndex.empty()) { return false; }
    if (isBalanceUpdated(pubkeyIndex)) {
      return true;
    }
    
    switch (transactionType) {
    case TransactionType::TRANSFER:
    case TransactionType::DEFERRED_TRANSFER:
    case TransactionType::REDEEM_DEFERRED_TRANSFER:
    case TransactionType::TIMEOUT_DEFERRED_TRANSFER:
      if (pubkeyIndex == getSender()) return true;
      [[fallthrough]]; 
    case TransactionType::CREATION:
      if (pubkeyIndex == getRecipient()) return true; // used for creation, transfer and deferred transfer
      break;
    case TransactionType::REGISTER_ADDRESS:
      if (pubkeyIndex == getRegisteredUser()) return true;
      if (pubkeyIndex == getRegisteredAccount()) return true;
      break;
    //case TransactionType::TIMEOUT_DEFERRED_TRANSFER:
      //throw GradidoUnhandledEnum("on ConfirmedGradidoTx::getInvolvedAddresses, removed because of simplify", "TransactionType", enum_name(transactionType).data());
      // break;
    case TransactionType::COMMUNITY_ROOT:
      if (pubkeyIndex == getCommunityRootPublicKey()) return true;
      if (pubkeyIndex == getAuf()) return true;
      if (pubkeyIndex == getGmw()) return true;
      break;
    default:
      throw GradidoUnhandledEnum("on ConfirmedGradidoTx::getInvolvedAddresses", "TransactionType", enum_name(transactionType).data());
    }
    if (coldData && pubkeyIndex.communityIdIndex == txCommunityIdIndex) {
      auto rawKey = pubkeyIndex.getRawKey();
      for (const auto& sigPair : coldData->signatureMap) {
        if (rawKey.isTheSame(sigPair.first)) return true;
      }
    }
    return false;
  }

  bool ConfirmedGradidoTx::hasCoinsFromCommunity(uint32_t coinColorCommunityId) const
  {
    for (int i = 0; i < accountBalanceCount; i++) {
      if (accountBalances[i].coinCommunityIdIndex == coinColorCommunityId) {
        return true;
      }
    }
    return false;
  }

  //! get sender public key index if it transfer or deferred transfer transaction else empty PublicKeyIndex
  PublicKeyIndex ConfirmedGradidoTx::getSender() const
  {
    uint32_t idx = 0;
    if (isTransfer() || isRedeemDeferredTransfer() || isTimeoutDeferredTransfer()) {
      idx = specific.transfer.senderPublicKeyIndex;
    }
    else if (isDeferredTransfer()) {
      idx = specific.deferredTransfer.senderPublicKeyIndex;
    }
    if (idx) {
      uint32_t commId = txCommunityIdIndex;
      if (isInboundCrossCommunityTx()) {
        if (!hasColdData()) {
          throw MissingColdDataException("missing cold data for getting full sender on inbound cross group tx", *this);
        }
        commId = coldData->pairingTxCommunityIdIndex;
      }
      return PublicKeyIndex{
        .communityIdIndex = commId,
        .publicKeyIndex = idx
      };
    }
    return PublicKeyIndex();
  }
  //! get recipient public key index if it is creation, transfer or deferred transfer transaction else empty PublicKeyIndex
  PublicKeyIndex ConfirmedGradidoTx::getRecipient() const
  {
    uint32_t idx = 0;
    if (isCreation()) {
      idx = specific.creation.recipientPublicKeyIndex;
    }
    if (isTransfer() || isRedeemDeferredTransfer() || isTimeoutDeferredTransfer()) {
      idx = specific.transfer.recipientPublicKeyIndex;
    }
    else if (isDeferredTransfer()) {
      idx = specific.deferredTransfer.recipientPublicKeyIndex;
    }
    if (idx) {
      uint32_t commId = txCommunityIdIndex;
      if (isOutboundCommunityTx()) {
        if (!hasColdData()) {
          throw MissingColdDataException("missing cold data for getting full recipient on outbound cross group tx", *this);
        }
        commId = coldData->pairingTxCommunityIdIndex;
      }
      return PublicKeyIndex{
        .communityIdIndex = commId,
        .publicKeyIndex = idx
      };
    }
    return PublicKeyIndex();
  }

  rich::AccountBalance ConfirmedGradidoTx::getAccountBalance(PublicKeyIndex publicKeyIndex, uint32_t coinCommunityIdIndex) const
  {
    if (publicKeyIndex.communityIdIndex != txCommunityIdIndex) {
      return rich::AccountBalance();
    }
    for (int i = 0; i < accountBalanceCount; i++) {
      if (accountBalances[i].publicKeyIndex == publicKeyIndex.publicKeyIndex) {
        if (!coinCommunityIdIndex || coinCommunityIdIndex && coinCommunityIdIndex == accountBalances[i].coinCommunityIdIndex) {
          return rich::AccountBalance(accountBalances[i], txCommunityIdIndex, getConfirmedAt());
        }
      }
    }
    return rich::AccountBalance();
  }

  MissingColdDataException::MissingColdDataException(const char* what, const ConfirmedGradidoTx& parent) noexcept
    : GradidoBlockchainException(what), mTxNr(parent.txNr), mTransactionType(parent.transactionType)
  {
  }

  string MissingColdDataException::getFullString() const 
  {
    string result = what();
    result += ", txNr: " + to_string(mTxNr);
    result += ", type: ";
    result += enum_name(mTransactionType);
    return result;
  }

}