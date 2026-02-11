#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/data/adapter/types.h"
#include "gradido_blockchain/data/compact/ConfirmedGradidoTx.h"
#include "gradido_blockchain/data/compact/ConfirmedGradidoTxCold.h"
#include "gradido_blockchain/data/compact/PublicKeyIndex.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

#include "loguru/loguru.hpp"
#include "magic_enum/magic_enum.hpp"


#include <optional>

using namespace magic_enum;
using std::optional, std::nullopt;
using std::set;

namespace gradido::data::compact {

  ConfirmedGradidoTx::ConfirmedGradidoTx()
    : txNr(0), confirmedAtSeconds(0), confirmedAtNanos(0), txCommunityIdIndex(0),
    crossGroupType(CrossGroupType::LOCAL), transactionType(TransactionType::NONE), balanceDerivationType(BalanceDerivationType::UNSPECIFIED),
    accountBalanceCount(0),
    coldData(nullptr)
  {
  }

  ConfirmedGradidoTx::~ConfirmedGradidoTx()
  {
    switch (transactionType) {
    case TransactionType::CREATION:
      specific.creation.~CreationTx();
      break;
    case TransactionType::TRANSFER:
      specific.transfer.~TransferTx();
      break;
    case TransactionType::DEFERRED_TRANSFER:
      specific.deferredTransfer.~DeferredTransferTx();
      break;
    case TransactionType::REDEEM_DEFERRED_TRANSFER:
      if (specific.redeemDeferredTransfer) {
        delete specific.redeemDeferredTransfer;
        specific.redeemDeferredTransfer = nullptr;
      }
      break;
    case TransactionType::TIMEOUT_DEFERRED_TRANSFER:
      specific.timeoutDeferredTransfer.~TimeoutDeferredTransferTx();
      break;
    case TransactionType::REGISTER_ADDRESS:
      specific.registerAddress.~RegisterAddressTx();
      break;
    case TransactionType::COMMUNITY_ROOT:
      specific.communityRoot.~CommunityRootTx();
      break;
    default:
      LOG_F(ERROR, "unhandled transaction type on ~ConfirmedGradidoTx() %s", enum_name(transactionType).data());
      break;
    }
    if (coldData) {
      delete coldData;
      coldData = nullptr;
    }
  }

  ConfirmedGradidoTx::ConfirmedGradidoTx(const ConfirmedGradidoTx& other)
    : txNr(other.txNr),
    confirmedAtSeconds(other.confirmedAtSeconds),
    confirmedAtNanos(other.confirmedAtNanos),
    txCommunityIdIndex(other.txCommunityIdIndex),
    crossGroupType(other.crossGroupType),
    transactionType(other.transactionType),
    balanceDerivationType(other.balanceDerivationType),
    accountBalanceCount(other.accountBalanceCount),
    coldData(nullptr)
  {
    // copy account balances
    for (uint8_t i = 0; i < accountBalanceCount; ++i) {
      accountBalances[i] = other.accountBalances[i];
    }

    // copy union based on discriminator
    switch (transactionType) {
    case TransactionType::CREATION:
      specific.creation = other.specific.creation;
      break;
    case TransactionType::TRANSFER:
      specific.transfer = other.specific.transfer;
      break;
    case TransactionType::DEFERRED_TRANSFER:
      specific.deferredTransfer = other.specific.deferredTransfer;
      break;
    case TransactionType::REDEEM_DEFERRED_TRANSFER:
      specific.redeemDeferredTransfer = new RedeemDeferredTransferTx{
        .deferredTransferTransactionNr = other.specific.redeemDeferredTransfer->deferredTransferTransactionNr,
        .transfer = other.specific.redeemDeferredTransfer->transfer
      };
      break;
    case TransactionType::TIMEOUT_DEFERRED_TRANSFER:
      specific.timeoutDeferredTransfer = other.specific.timeoutDeferredTransfer;
      break;
    case TransactionType::REGISTER_ADDRESS:
      specific.registerAddress = other.specific.registerAddress;
      break;
    case TransactionType::COMMUNITY_ROOT:
      specific.communityRoot = other.specific.communityRoot;
      break;
    default:
      throw GradidoUnhandledEnum("on copy ConfirmedGradidoTx", "TransactionType", enum_name(transactionType).data());
      break;
    }

    // deep copy cold data
    if (other.coldData) {
      coldData = new ConfirmedGradidoTxCold(*other.coldData);
    }
  }

  ConfirmedGradidoTx::ConfirmedGradidoTx(ConfirmedGradidoTx&& other)
    : txNr(other.txNr),
    confirmedAtSeconds(other.confirmedAtSeconds),
    confirmedAtNanos(other.confirmedAtNanos),
    txCommunityIdIndex(other.txCommunityIdIndex),
    crossGroupType(other.crossGroupType),
    transactionType(other.transactionType),
    balanceDerivationType(other.balanceDerivationType),
    accountBalanceCount(other.accountBalanceCount),
    coldData(other.coldData)
  {
    // move account balances
    for (uint8_t i = 0; i < accountBalanceCount; ++i) {
      accountBalances[i] = std::move(other.accountBalances[i]);
    }

    // steal union content
    switch (transactionType) {
    case TransactionType::CREATION:
      specific.creation = other.specific.creation;
      break;
    case TransactionType::TRANSFER:
      specific.transfer = other.specific.transfer;
      break;
    case TransactionType::DEFERRED_TRANSFER:
      specific.deferredTransfer = other.specific.deferredTransfer;
      break;
    case TransactionType::REDEEM_DEFERRED_TRANSFER:
      specific.redeemDeferredTransfer = other.specific.redeemDeferredTransfer;
      other.specific.redeemDeferredTransfer = nullptr;
      break;
    case TransactionType::TIMEOUT_DEFERRED_TRANSFER:
      specific.timeoutDeferredTransfer = other.specific.timeoutDeferredTransfer;
      break;
    case TransactionType::REGISTER_ADDRESS:
      specific.registerAddress = other.specific.registerAddress;
      break;
    case TransactionType::COMMUNITY_ROOT:
      specific.communityRoot = other.specific.communityRoot;
      break;
    default:
      throw GradidoUnhandledEnum("on move ConfirmedGradidoTx", "TransactionType", enum_name(transactionType).data());
      break;
    }

    // neuter the source
    other.coldData = nullptr;
    other.accountBalanceCount = 0;
    other.transactionType = TransactionType::NONE;
  }
  // move
  ConfirmedGradidoTx& ConfirmedGradidoTx::operator=(ConfirmedGradidoTx&& other)
  {
    txNr = other.txNr; 
    confirmedAtSeconds = other.confirmedAtSeconds;
    confirmedAtNanos = other.confirmedAtNanos;
    txCommunityIdIndex = other.txCommunityIdIndex;
    crossGroupType = other.crossGroupType;
    transactionType = other.transactionType;
    balanceDerivationType = other.balanceDerivationType;
    accountBalanceCount = other.accountBalanceCount;
    coldData = other.coldData;

    // move account balances
    for (uint8_t i = 0; i < accountBalanceCount; ++i) {
      accountBalances[i] = std::move(other.accountBalances[i]);
    }

    // steal union content
    switch (transactionType) {
    case TransactionType::CREATION:
      specific.creation = other.specific.creation;
      break;
    case TransactionType::TRANSFER:
      specific.transfer = other.specific.transfer;
      break;
    case TransactionType::DEFERRED_TRANSFER:
      specific.deferredTransfer = other.specific.deferredTransfer;
      break;
    case TransactionType::REDEEM_DEFERRED_TRANSFER:
      specific.redeemDeferredTransfer = other.specific.redeemDeferredTransfer;
      other.specific.redeemDeferredTransfer = nullptr;
      break;
    case TransactionType::TIMEOUT_DEFERRED_TRANSFER:
      specific.timeoutDeferredTransfer = other.specific.timeoutDeferredTransfer;
      break;
    case TransactionType::REGISTER_ADDRESS:
      specific.registerAddress = other.specific.registerAddress;
      break;
    case TransactionType::COMMUNITY_ROOT:
      specific.communityRoot = other.specific.communityRoot;
      break;
    default:
      throw GradidoUnhandledEnum("on move ConfirmedGradidoTx", "TransactionType", enum_name(transactionType).data());
      break;
    }

    // neuter the source
    other.coldData = nullptr;
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
    crossGroupType = other.crossGroupType;
    transactionType = other.transactionType;
    balanceDerivationType = other.balanceDerivationType;
    accountBalanceCount = other.accountBalanceCount;
    
    // copy account balances
    for (uint8_t i = 0; i < accountBalanceCount; ++i) {
      accountBalances[i] = other.accountBalances[i];
    }

    // copy union based on discriminator
    switch (transactionType) {
    case TransactionType::CREATION:
      specific.creation = other.specific.creation;
      break;
    case TransactionType::TRANSFER:
      specific.transfer = other.specific.transfer;
      break;
    case TransactionType::DEFERRED_TRANSFER:
      specific.deferredTransfer = other.specific.deferredTransfer;
      break;
    case TransactionType::REDEEM_DEFERRED_TRANSFER:
      specific.redeemDeferredTransfer = new RedeemDeferredTransferTx{
        .deferredTransferTransactionNr = other.specific.redeemDeferredTransfer->deferredTransferTransactionNr,
        .transfer = other.specific.redeemDeferredTransfer->transfer
      };
      break;
    case TransactionType::TIMEOUT_DEFERRED_TRANSFER:
      specific.timeoutDeferredTransfer = other.specific.timeoutDeferredTransfer;
      break;
    case TransactionType::REGISTER_ADDRESS:
      specific.registerAddress = other.specific.registerAddress;
      break;
    case TransactionType::COMMUNITY_ROOT:
      specific.communityRoot = other.specific.communityRoot;
      break;
    default:
      throw GradidoUnhandledEnum("on copy ConfirmedGradidoTx", "TransactionType", enum_name(transactionType).data());
      break;
    }

    // deep copy cold data
    if (other.coldData) {
      coldData = new ConfirmedGradidoTxCold(*other.coldData);
    }

    return *this;
  }

  ConfirmedGradidoTx ConfirmedGradidoTx::fromGrdw(
    const grdw_confirmed_transaction* tx,
    const grdw_transaction_body* body,
    uint32_t blockchainCommunityIdIndex,
    bool loadColdData/* = true */
  ) {
    auto confirmedTx = fromGrdwConfirmedTransaction(tx, blockchainCommunityIdIndex, loadColdData);
    confirmedTx.fillFromGrdwTransactionBody(body);
    return confirmedTx;
  }

  ConfirmedGradidoTx ConfirmedGradidoTx::fromGrdwConfirmedTransaction(
    const grdw_confirmed_transaction* tx,
    uint32_t blockchainCommunityIdIndex,
    bool loadColdData/* = true*/
  ) {
    ConfirmedGradidoTx confirmedTx;
    confirmedTx.txNr = tx->id;
    confirmedTx.confirmedAtSeconds = tx->confirmed_at.seconds;
    confirmedTx.confirmedAtNanos = tx->confirmed_at.nanos;
    confirmedTx.txCommunityIdIndex = blockchainCommunityIdIndex;
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
      if (tx_account_balance.community_id) {
        accountBalance.coinCommunityIdIndex = g_appContext->getOrAddCommunityIdIndex(tx_account_balance.community_id);
      }
      accountBalance.publicKeyIndex = g_appContext->getOrAddPublicKeyIndex(blockchainCommunityIdIndex, tx_account_balance.pubkey);
    }
    
    if (loadColdData) {
      confirmedTx.coldData = ConfirmedGradidoTxCold::fromGrdwConfirmedTransaction(tx);
    }
    return confirmedTx;
  }

  void ConfirmedGradidoTx::fillFromGrdwTransactionBody(const grdw_transaction_body* body)
  {
    crossGroupType = adapter::fromGrdw(body->type);
    transactionType = adapter::fromGrdw(body->transaction_type);
    
    uint32_t senderCommunityIdIndex = txCommunityIdIndex;
    uint32_t recipientCommunityIdIndex = txCommunityIdIndex;
    if (crossGroupType != CrossGroupType::LOCAL) {
      switch (crossGroupType) {
      case CrossGroupType::OUTBOUND:
        recipientCommunityIdIndex = g_appContext->getOrAddCommunityIdIndex(body->other_group);
        break;
      case CrossGroupType::INBOUND:
        senderCommunityIdIndex = g_appContext->getOrAddCommunityIdIndex(body->other_group);
        break;
      default:
        throw GradidoUnhandledEnum(
          "compact confirmed transaction from grdw, not implemented yet",
          enum_type_name<decltype(crossGroupType)>().data(),
          enum_name(crossGroupType).data()
        );
      }
    }
    switch (transactionType) {
    case TransactionType::TRANSFER:
      specific.transfer = TransferTx::fromGrdw(body->data.transfer, senderCommunityIdIndex, recipientCommunityIdIndex);
      break;
    case TransactionType::CREATION:
      specific.creation = {
        .amountGddCent = body->data.creation->recipient.amount,
        .recipientPublicKeyIndex = PublicKeyIndex::fromPublicKey(recipientCommunityIdIndex, body->data.creation->recipient.pubkey),
        .targetDateSeconds = body->data.creation->target_date.seconds
      };
      break;
    case TransactionType::REGISTER_ADDRESS:
      if (body->data.register_address->derivation_index != static_cast<uint16_t>(body->data.register_address->derivation_index)) {
        throw GradidoNodeInvalidDataException("derivation index exceed uint16_t");
      }
      specific.registerAddress = {
        .addressType = adapter::fromGrdw(body->data.register_address->address_type),
        .derivationIndex = static_cast<uint16_t>(body->data.register_address->derivation_index),
        .nameHashIndex = g_appContext->getOrAddUserNameHashIndex(body->data.register_address->name_hash),
        .userPublicKeyIndex = PublicKeyIndex::fromPublicKey(senderCommunityIdIndex, body->data.register_address->user_pubkey),
        .accountPublicKeyIndex = PublicKeyIndex::fromPublicKey(senderCommunityIdIndex, body->data.register_address->account_pubkey)
      };
      break;
    case TransactionType::DEFERRED_TRANSFER:
      if (crossGroupType != CrossGroupType::LOCAL) {
        throw GradidoNodeInvalidDataException("deferred transfer (currently) don't work cross community");
      }
      if (body->data.deferred_transfer->transfer.sender.community_id) {
        throw GradidoNodeInvalidDataException("deferred transfer not expected field coin community id filled");
      }
      specific.deferredTransfer = {
        .amountGddCent = body->data.deferred_transfer->transfer.sender.amount,
        .senderPublicKeyIndex = g_appContext->getOrAddPublicKeyIndex(txCommunityIdIndex, body->data.deferred_transfer->transfer.sender.pubkey),
        .recipientPublicKeyIndex = g_appContext->getOrAddPublicKeyIndex(txCommunityIdIndex, body->data.deferred_transfer->transfer.recipient),
        .timeoutDurationSeconds = body->data.deferred_transfer->timeout_duration
      };
      break;
    case TransactionType::REDEEM_DEFERRED_TRANSFER:
      specific.redeemDeferredTransfer =
        RedeemDeferredTransferTx::fromGrdw(body->data.redeem_deferred_transfer, senderCommunityIdIndex, recipientCommunityIdIndex)
        ;
      break;
    case TransactionType::TIMEOUT_DEFERRED_TRANSFER:
      specific.timeoutDeferredTransfer = {
        .deferredTransferTransactionNr = {
          .nr = body->data.timeout_deferred_transfer->deferred_transfer_transaction_nr,
          .communityIdIndex = senderCommunityIdIndex
        }
      };
      break;
    case TransactionType::COMMUNITY_ROOT:
      specific.communityRoot = {
        .publicKeyIndex = PublicKeyIndex::fromPublicKey(senderCommunityIdIndex, body->data.community_root->pubkey),
        .gmwPublicKeyIndex = PublicKeyIndex::fromPublicKey(senderCommunityIdIndex, body->data.community_root->gmw_pubkey),
        .aufPublicKeyIndex = PublicKeyIndex::fromPublicKey(senderCommunityIdIndex, body->data.community_root->auf_pubkey)
      };
      break;
    default:
      throw GradidoUnhandledEnum("on ConfirmedGradidoTx::fromGrdw", "TransactionType", enum_name(transactionType).data());
    }
    if (coldData) {
      coldData->fillFromGrdwTransactionBody(body);
    }
  }

  optional<uint32_t> ConfirmedGradidoTx::getCoinCommunityId() const
  {
    optional<uint32_t> result = nullopt;
    for (int i = 0; i < accountBalanceCount; i++) {
      if (accountBalances[i].coinCommunityIdIndex != txCommunityIdIndex) {
        if (result) {
          throw GradidoNotImplementedException("cannot handle more than one differentiated coin community id index");
        }
        result = accountBalances[i].coinCommunityIdIndex;
      }
    }
    return result;
  }

  set<PublicKeyIndex> ConfirmedGradidoTx::getInvolvedAddresses() const
  {
    set<PublicKeyIndex> result;
    for (int i = 0; i < accountBalanceCount; i++) {
      result.insert({
          .communityIdIndex = txCommunityIdIndex,
          .publicKeyIndex = accountBalances[i].publicKeyIndex
        }
      );
    }
    if (coldData) {
      for (const auto& sigPair : coldData->signatureMap) {
        result.insert({
            .communityIdIndex = txCommunityIdIndex,
            .publicKeyIndex = g_appContext->getOrAddPublicKeyIndex(txCommunityIdIndex, sigPair.first)
          }
        );
      }
    }
    switch (transactionType) {
    case TransactionType::TRANSFER:
      specific.transfer.getInvolvedAddresses(result);
      break;
    case TransactionType::CREATION:
      result.insert(specific.creation.recipientPublicKeyIndex);
      break;
    case TransactionType::REGISTER_ADDRESS:
      result.insert(specific.registerAddress.accountPublicKeyIndex);
      result.insert(specific.registerAddress.userPublicKeyIndex);
      break;
    case TransactionType::DEFERRED_TRANSFER:
      result.insert({
          .communityIdIndex = txCommunityIdIndex,
          .publicKeyIndex = specific.deferredTransfer.senderPublicKeyIndex
        }
      );
      result.insert({
          .communityIdIndex = txCommunityIdIndex,
          .publicKeyIndex = specific.deferredTransfer.recipientPublicKeyIndex
        }
      );
      break;
    case TransactionType::REDEEM_DEFERRED_TRANSFER:
      specific.redeemDeferredTransfer->transfer.getInvolvedAddresses(result);
      break;
    case TransactionType::TIMEOUT_DEFERRED_TRANSFER:
      break;
    case TransactionType::COMMUNITY_ROOT:
      result.insert(specific.communityRoot.publicKeyIndex);
      result.insert(specific.communityRoot.gmwPublicKeyIndex);
      result.insert(specific.communityRoot.aufPublicKeyIndex);
      break;
    default: 
      throw GradidoUnhandledEnum("on ConfirmedGradidoTx::getInvolvedAddresses", "TransactionType", enum_name(transactionType).data());
    }
    return result;
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
}