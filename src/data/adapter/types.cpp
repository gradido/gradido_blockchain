#include "gradido_blockchain/data/adapter/types.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

#include <string>

using std::to_string;

namespace gradido::data::adapter {
  MemoKeyType fromGrdw(grdt_memo_key type) 
  {
    switch(type) {
      case GRDT_MEMO_KEY_SHARED_SECRET:
        return MemoKeyType::SHARED_SECRET;
      case GRDT_MEMO_KEY_COMMUNITY_SECRET:
        return MemoKeyType::COMMUNITY_SECRET;
      case GRDT_MEMO_KEY_PLAIN:
        return MemoKeyType::PLAIN;
      default:
        throw GradidoUnhandledEnum("error converting memo key from grdw (C-Interface)", "MemoKeyType", to_string(type).c_str());
    }
  }  
  grdt_memo_key toGrdw(MemoKeyType type) {
    switch(type) {
      case MemoKeyType::SHARED_SECRET:
        return GRDT_MEMO_KEY_SHARED_SECRET;
      case MemoKeyType::COMMUNITY_SECRET:
        return GRDT_MEMO_KEY_COMMUNITY_SECRET;
      case MemoKeyType::PLAIN:
        return GRDT_MEMO_KEY_PLAIN;
      default:
        throw GradidoUnhandledEnum("error converting memo key to grdw (C-Interface)", "MemoKeyType", to_string(static_cast<uint8_t>(type)).c_str());
    }
  }

  CrossGroupType fromGrdw(grdt_cross_group type)
  {
    switch(type) {
      case GRDT_CROSS_GROUP_LOCAL:
        return CrossGroupType::LOCAL;
      case GRDT_CROSS_GROUP_INBOUND:
        return CrossGroupType::INBOUND;
      case GRDT_CROSS_GROUP_OUTBOUND:
        return CrossGroupType::OUTBOUND;
      case GRDT_CROSS_GROUP_CROSS:
        return CrossGroupType::CROSS;
      default:
        throw GradidoUnhandledEnum("error converting cross group type from grdw (C-Interface)", "CrossGroupType", to_string(type).c_str());
    }
  }
  grdt_cross_group toGrdw(CrossGroupType type)
  {
    switch(type) {
      case CrossGroupType::LOCAL:
        return GRDT_CROSS_GROUP_LOCAL;
      case CrossGroupType::INBOUND:
        return GRDT_CROSS_GROUP_INBOUND;
      case CrossGroupType::OUTBOUND:
        return GRDT_CROSS_GROUP_OUTBOUND;
      case CrossGroupType::CROSS:
        return GRDT_CROSS_GROUP_CROSS;
      default:
        throw GradidoUnhandledEnum("error converting cross group type to grdw (C-Interface)", "CrossGroupType", to_string(static_cast<uint8_t>(type)).c_str());
    }
  }
  
  TransactionType fromGrdw(grdt_transaction type)
  {
    switch(type) {
      case GRDT_TRANSACTION_NONE:
        return TransactionType::NONE;
      case GRDT_TRANSACTION_CREATION:
        return TransactionType::CREATION;
      case GRDT_TRANSACTION_TRANSFER:
        return TransactionType::TRANSFER;
      case GRDT_TRANSACTION_COMMUNITY_FRIENDS_UPDATE:
        return TransactionType::COMMUNITY_FRIENDS_UPDATE;
      case GRDT_TRANSACTION_REGISTER_ADDRESS:
        return TransactionType::REGISTER_ADDRESS;
      case GRDT_TRANSACTION_DEFERRED_TRANSFER:
        return TransactionType::DEFERRED_TRANSFER;
      case GRDT_TRANSACTION_COMMUNITY_ROOT:
        return TransactionType::COMMUNITY_ROOT;
      case GRDT_TRANSACTION_REDEEM_DEFERRED_TRANSFER:
        return TransactionType::REDEEM_DEFERRED_TRANSFER;
      case GRDT_TRANSACTION_TIMEOUT_DEFERRED_TRANSFER:
        return TransactionType::TIMEOUT_DEFERRED_TRANSFER;
      default:
        throw GradidoUnhandledEnum("error converting transaction type from grdw (C-Interface)", "TransactionType", to_string(type).c_str());
    }
  }
  grdt_transaction toGrdw(TransactionType type)
  {
    switch(type) {
      case TransactionType::NONE:
        return GRDT_TRANSACTION_NONE;
      case TransactionType::CREATION:
        return GRDT_TRANSACTION_CREATION;
      case TransactionType::TRANSFER:
        return GRDT_TRANSACTION_TRANSFER;
      case TransactionType::COMMUNITY_FRIENDS_UPDATE:
        return GRDT_TRANSACTION_COMMUNITY_FRIENDS_UPDATE;
      case TransactionType::REGISTER_ADDRESS:
        return GRDT_TRANSACTION_REGISTER_ADDRESS;
      case TransactionType::DEFERRED_TRANSFER:
        return GRDT_TRANSACTION_DEFERRED_TRANSFER;
      case TransactionType::COMMUNITY_ROOT:
        return GRDT_TRANSACTION_COMMUNITY_ROOT;
      case TransactionType::REDEEM_DEFERRED_TRANSFER:
        return GRDT_TRANSACTION_REDEEM_DEFERRED_TRANSFER;
      case TransactionType::TIMEOUT_DEFERRED_TRANSFER:
        return GRDT_TRANSACTION_TIMEOUT_DEFERRED_TRANSFER;
      default:
        throw GradidoUnhandledEnum("error converting transaction type to grdw (C-Interface)", "TransactionType", to_string(static_cast<uint8_t>(type)).c_str());
    }
  }
  
  AddressType fromGrdw(grdt_address type)
  {
    switch(type) {
      case GRDT_ADDRESS_NONE:
        return AddressType::NONE;
      case GRDT_ADDRESS_COMMUNITY_HUMAN:
        return AddressType::COMMUNITY_HUMAN;
      case GRDT_ADDRESS_COMMUNITY_GMW:
        return AddressType::COMMUNITY_GMW;
      case GRDT_ADDRESS_COMMUNITY_AUF:
        return AddressType::COMMUNITY_AUF;
      case GRDT_ADDRESS_COMMUNITY_PROJECT:
        return AddressType::COMMUNITY_PROJECT;
      case GRDT_ADDRESS_SUBACCOUNT:
        return AddressType::SUBACCOUNT;
      case GRDT_ADDRESS_CRYPTO_ACCOUNT:
        return AddressType::CRYPTO_ACCOUNT;
      default:
        throw GradidoUnhandledEnum("error converting address type from grdw (C-Interface)", "AddressType", to_string(type).c_str());
    }
  }
  grdt_address toGrdw(AddressType type)
  {
    switch(type) {
      case AddressType::NONE:
        return GRDT_ADDRESS_NONE;
      case AddressType::COMMUNITY_HUMAN:
        return GRDT_ADDRESS_COMMUNITY_HUMAN;
      case AddressType::COMMUNITY_GMW:
        return GRDT_ADDRESS_COMMUNITY_GMW;
      case AddressType::COMMUNITY_AUF:
        return GRDT_ADDRESS_COMMUNITY_AUF;
      case AddressType::COMMUNITY_PROJECT:
        return GRDT_ADDRESS_COMMUNITY_PROJECT;
      case AddressType::SUBACCOUNT:
        return GRDT_ADDRESS_SUBACCOUNT;
      case AddressType::CRYPTO_ACCOUNT:
        return GRDT_ADDRESS_CRYPTO_ACCOUNT;
      default:
        throw GradidoUnhandledEnum("error converting address type to grdw (C-Interface)", "AddressType", to_string(static_cast<uint8_t>(type)).c_str());
    }
  }

  LedgerAnchor::Type fromGrdw(grdt_ledger_anchor type)
  {
    switch (type) {
      case GRDT_LEDGER_ANCHOR_UNSPECIFIED:
        return LedgerAnchor::Type::UNSPECIFIED;
      case GRDT_LEDGER_ANCHOR_HIERO_TRANSACTION_ID:
        return LedgerAnchor::Type::HIERO_TRANSACTION_ID;
      case GRDT_LEDGER_ANCHOR_LEGACY_GRADIDO_DB_TRANSACTION_ID:
        return LedgerAnchor::Type::LEGACY_GRADIDO_DB_TRANSACTION_ID;
      case GRDT_LEDGER_ANCHOR_NODE_TRIGGER_TRANSACTION_ID:
        return LedgerAnchor::Type::NODE_TRIGGER_TRANSACTION_ID;
      case GRDT_LEDGER_ANCHOR_LEGACY_GRADIDO_DB_COMMUNITY_ID:
        return LedgerAnchor::Type::LEGACY_GRADIDO_DB_COMMUNITY_ID;
      case GRDT_LEDGER_ANCHOR_LEGACY_GRADIDO_DB_USER_ID:
        return LedgerAnchor::Type::LEGACY_GRADIDO_DB_USER_ID;
      case GRDT_LEDGER_ANCHOR_LEGACY_GRADIDO_DB_CONTRIBUTION_ID:
        return LedgerAnchor::Type::LEGACY_GRADIDO_DB_CONTRIBUTION_ID;
      case GRDT_LEDGER_ANCHOR_LEGACY_GRADIDO_DB_TRANSACTION_LINK_ID:
        return LedgerAnchor::Type::LEGACY_GRADIDO_DB_TRANSACTION_LINK_ID;
      default:
        throw GradidoUnhandledEnum("error converting ledger anchor type from grdw (C-Interface)", "LedgerAnchor::Type", to_string(static_cast<uint8_t>(type)).c_str());
    }
  }

  grdt_ledger_anchor toGrdw(LedgerAnchor::Type type)
  {
    switch (type) {
      case LedgerAnchor::Type::UNSPECIFIED:
        return GRDT_LEDGER_ANCHOR_UNSPECIFIED;
      case LedgerAnchor::Type::HIERO_TRANSACTION_ID:
        return GRDT_LEDGER_ANCHOR_HIERO_TRANSACTION_ID;
      case LedgerAnchor::Type::LEGACY_GRADIDO_DB_TRANSACTION_ID:
        return GRDT_LEDGER_ANCHOR_LEGACY_GRADIDO_DB_TRANSACTION_ID;
      case LedgerAnchor::Type::NODE_TRIGGER_TRANSACTION_ID:
        return GRDT_LEDGER_ANCHOR_NODE_TRIGGER_TRANSACTION_ID;
      case LedgerAnchor::Type::LEGACY_GRADIDO_DB_COMMUNITY_ID:
        return GRDT_LEDGER_ANCHOR_LEGACY_GRADIDO_DB_COMMUNITY_ID;
      case LedgerAnchor::Type::LEGACY_GRADIDO_DB_USER_ID:
        return GRDT_LEDGER_ANCHOR_LEGACY_GRADIDO_DB_USER_ID;
      case LedgerAnchor::Type::LEGACY_GRADIDO_DB_CONTRIBUTION_ID:
        return GRDT_LEDGER_ANCHOR_LEGACY_GRADIDO_DB_CONTRIBUTION_ID;
      case LedgerAnchor::Type::LEGACY_GRADIDO_DB_TRANSACTION_LINK_ID:
        return GRDT_LEDGER_ANCHOR_LEGACY_GRADIDO_DB_TRANSACTION_LINK_ID;
      default:
        throw GradidoUnhandledEnum("error converting ledger anchor type to grdw (C-Interface)", "LedgerAnchor::Type", to_string(static_cast<uint8_t>(type)).c_str());
    }
  }

  BalanceDerivationType fromGrdw(grdt_balance_derivation type)
  {
    switch(type) {
      case GRDT_BALANCE_DERIVATION_UNSPECIFIED:
        return BalanceDerivationType::UNSPECIFIED;
      case GRDT_BALANCE_DERIVATION_NODE:
        return BalanceDerivationType::NODE;
      case GRDT_BALANCE_DERIVATION_EXTERN:
        return BalanceDerivationType::EXTERN;
      default:
        throw GradidoUnhandledEnum("error converting balance derivation type from grdw (C-Interface)", "BalanceDerivationType", to_string(static_cast<uint8_t>(type)).c_str());
    }

  }
  grdt_balance_derivation toGrdw(BalanceDerivationType type)
  {
    switch(type) {
      case BalanceDerivationType::UNSPECIFIED:
        return GRDT_BALANCE_DERIVATION_UNSPECIFIED;
      case BalanceDerivationType::NODE:
        return GRDT_BALANCE_DERIVATION_NODE;
      case BalanceDerivationType::EXTERN:
        return GRDT_BALANCE_DERIVATION_EXTERN;
      default:
        throw GradidoUnhandledEnum("error converting balance derivation type to grdw (C-Interface)", "BalanceDerivationType", to_string(static_cast<uint8_t>(type)).c_str());
    }
  }
}