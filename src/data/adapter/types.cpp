#include "gradido_blockchain/data/adapter/types.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

#include <string>

using std::to_string;

namespace gradido::data::adapter {
  MemoKeyType fromGrdw(grdw_memo_key_type type) 
  {
    switch(type) {
      case GRDW_MEMO_KEY_TYPE_SHARED_SECRET:
        return MemoKeyType::SHARED_SECRET;
      case GRDW_MEMO_KEY_TYPE_COMMUNITY_SECRET:
        return MemoKeyType::COMMUNITY_SECRET;
      case GRDW_MEMO_KEY_TYPE_PLAIN:
        return MemoKeyType::PLAIN;
      default:
        throw GradidoUnhandledEnum("error converting memo key from grdw (C-Interface)", "MemoKeyType", to_string(type).c_str());
    }
  }  
  grdw_memo_key_type toGrdw(MemoKeyType type) {
    switch(type) {
      case MemoKeyType::SHARED_SECRET:
        return GRDW_MEMO_KEY_TYPE_SHARED_SECRET;
      case MemoKeyType::COMMUNITY_SECRET:
        return GRDW_MEMO_KEY_TYPE_COMMUNITY_SECRET;
      case MemoKeyType::PLAIN:
        return GRDW_MEMO_KEY_TYPE_PLAIN;
      default:
        throw GradidoUnhandledEnum("error converting memo key to grdw (C-Interface)", "MemoKeyType", to_string(static_cast<uint8_t>(type)).c_str());
    }
  }

  CrossGroupType fromGrdw(grdd_cross_group_type type)
  {
    switch(type) {
      case GRDD_CROSS_GROUP_TYPE_LOCAL:
        return CrossGroupType::LOCAL;
      case GRDD_CROSS_GROUP_TYPE_INBOUND:
        return CrossGroupType::INBOUND;
      case GRDD_CROSS_GROUP_TYPE_OUTBOUND:
        return CrossGroupType::OUTBOUND;
      case GRDD_CROSS_GROUP_TYPE_CROSS:
        return CrossGroupType::CROSS;
      default:
        throw GradidoUnhandledEnum("error converting cross group type from grdw (C-Interface)", "CrossGroupType", to_string(type).c_str());
    }
  }
  grdd_cross_group_type toGrdw(CrossGroupType type)
  {
    switch(type) {
      case CrossGroupType::LOCAL:
        return GRDD_CROSS_GROUP_TYPE_LOCAL;
      case CrossGroupType::INBOUND:
        return GRDD_CROSS_GROUP_TYPE_INBOUND;
      case CrossGroupType::OUTBOUND:
        return GRDD_CROSS_GROUP_TYPE_OUTBOUND;
      case CrossGroupType::CROSS:
        return GRDD_CROSS_GROUP_TYPE_CROSS;
      default:
        throw GradidoUnhandledEnum("error converting cross group type to grdw (C-Interface)", "CrossGroupType", to_string(static_cast<uint8_t>(type)).c_str());
    }
  }
  
  TransactionType fromGrdw(grdd_transaction_type type)
  {
    switch(type) {
      case GRDD_TRANSACTION_TYPE_NONE:
        return TransactionType::NONE;
      case GRDD_TRANSACTION_TYPE_CREATION:
        return TransactionType::CREATION;
      case GRDD_TRANSACTION_TYPE_TRANSFER:
        return TransactionType::TRANSFER;
      case GRDD_TRANSACTION_TYPE_COMMUNITY_FRIENDS_UPDATE:
        return TransactionType::COMMUNITY_FRIENDS_UPDATE;
      case GRDD_TRANSACTION_TYPE_REGISTER_ADDRESS:
        return TransactionType::REGISTER_ADDRESS;
      case GRDD_TRANSACTION_TYPE_DEFERRED_TRANSFER:
        return TransactionType::DEFERRED_TRANSFER;
      case GRDD_TRANSACTION_TYPE_COMMUNITY_ROOT:
        return TransactionType::COMMUNITY_ROOT;
      case GRDD_TRANSACTION_TYPE_REDEEM_DEFERRED_TRANSFER:
        return TransactionType::REDEEM_DEFERRED_TRANSFER;
      case GRDD_TRANSACTION_TYPE_TIMEOUT_DEFERRED_TRANSFER:
        return TransactionType::TIMEOUT_DEFERRED_TRANSFER;
      default:
        throw GradidoUnhandledEnum("error converting transaction type from grdw (C-Interface)", "TransactionType", to_string(type).c_str());
    }
  }
  grdd_transaction_type toGrdw(TransactionType type)
  {
    switch(type) {
      case TransactionType::NONE:
        return GRDD_TRANSACTION_TYPE_NONE;
      case TransactionType::CREATION:
        return GRDD_TRANSACTION_TYPE_CREATION;
      case TransactionType::TRANSFER:
        return GRDD_TRANSACTION_TYPE_TRANSFER;
      case TransactionType::COMMUNITY_FRIENDS_UPDATE:
        return GRDD_TRANSACTION_TYPE_COMMUNITY_FRIENDS_UPDATE;
      case TransactionType::REGISTER_ADDRESS:
        return GRDD_TRANSACTION_TYPE_REGISTER_ADDRESS;
      case TransactionType::DEFERRED_TRANSFER:
        return GRDD_TRANSACTION_TYPE_DEFERRED_TRANSFER;
      case TransactionType::COMMUNITY_ROOT:
        return GRDD_TRANSACTION_TYPE_COMMUNITY_ROOT;
      case TransactionType::REDEEM_DEFERRED_TRANSFER:
        return GRDD_TRANSACTION_TYPE_REDEEM_DEFERRED_TRANSFER;
      case TransactionType::TIMEOUT_DEFERRED_TRANSFER:
        return GRDD_TRANSACTION_TYPE_TIMEOUT_DEFERRED_TRANSFER;
      default:
        throw GradidoUnhandledEnum("error converting transaction type to grdw (C-Interface)", "TransactionType", to_string(static_cast<uint8_t>(type)).c_str());
    }
  }
  
  AddressType fromGrdw(grdd_address_type type)
  {
    switch(type) {
      case GRDD_ADDRESS_TYPE_NONE:
        return AddressType::NONE;
      case GRDD_ADDRESS_TYPE_COMMUNITY_HUMAN:
        return AddressType::COMMUNITY_HUMAN;
      case GRDD_ADDRESS_TYPE_COMMUNITY_GMW:
        return AddressType::COMMUNITY_GMW;
      case GRDD_ADDRESS_TYPE_COMMUNITY_AUF:
        return AddressType::COMMUNITY_AUF;
      case GRDD_ADDRESS_TYPE_COMMUNITY_PROJECT:
        return AddressType::COMMUNITY_PROJECT;
      case GRDD_ADDRESS_TYPE_SUBACCOUNT:
        return AddressType::SUBACCOUNT;
      case GRDD_ADDRESS_TYPE_CRYPTO_ACCOUNT:
        return AddressType::CRYPTO_ACCOUNT;
      default:
        throw GradidoUnhandledEnum("error converting address type from grdw (C-Interface)", "AddressType", to_string(type).c_str());
    }
  }
  grdd_address_type toGrdw(AddressType type)
  {
    switch(type) {
      case AddressType::NONE:
        return GRDD_ADDRESS_TYPE_NONE;
      case AddressType::COMMUNITY_HUMAN:
        return GRDD_ADDRESS_TYPE_COMMUNITY_HUMAN;
      case AddressType::COMMUNITY_GMW:
        return GRDD_ADDRESS_TYPE_COMMUNITY_GMW;
      case AddressType::COMMUNITY_AUF:
        return GRDD_ADDRESS_TYPE_COMMUNITY_AUF;
      case AddressType::COMMUNITY_PROJECT:
        return GRDD_ADDRESS_TYPE_COMMUNITY_PROJECT;
      case AddressType::SUBACCOUNT:
        return GRDD_ADDRESS_TYPE_SUBACCOUNT;
      case AddressType::CRYPTO_ACCOUNT:
        return GRDD_ADDRESS_TYPE_CRYPTO_ACCOUNT;
      default:
        throw GradidoUnhandledEnum("error converting address type to grdw (C-Interface)", "AddressType", to_string(static_cast<uint8_t>(type)).c_str());
    }
  }

  LedgerAnchor::Type fromGrdw(grdw_ledger_anchor_type type)
  {
    switch (type) {
      case GRDW_LEDGER_ANCHOR_TYPE_UNSPECIFIED:
        return LedgerAnchor::Type::UNSPECIFIED;
      case GRDW_LEDGER_ANCHOR_TYPE_HIERO_TRANSACTION_ID:
        return LedgerAnchor::Type::HIERO_TRANSACTION_ID;
      case GRDW_LEDGER_ANCHOR_TYPE_LEGACY_GRADIDO_DB_TRANSACTION_ID:
        return LedgerAnchor::Type::LEGACY_GRADIDO_DB_TRANSACTION_ID;
      case GRDW_LEDGER_ANCHOR_TYPE_NODE_TRIGGER_TRANSACTION_ID:
        return LedgerAnchor::Type::NODE_TRIGGER_TRANSACTION_ID;
      case GRDW_LEDGER_ANCHOR_TYPE_LEGACY_GRADIDO_DB_COMMUNITY_ID:
        return LedgerAnchor::Type::LEGACY_GRADIDO_DB_COMMUNITY_ID;
      case GRDW_LEDGER_ANCHOR_TYPE_LEGACY_GRADIDO_DB_USER_ID:
        return LedgerAnchor::Type::LEGACY_GRADIDO_DB_USER_ID;
      case GRDW_LEDGER_ANCHOR_TYPE_LEGACY_GRADIDO_DB_CONTRIBUTION_ID:
        return LedgerAnchor::Type::LEGACY_GRADIDO_DB_CONTRIBUTION_ID;
      case GRDW_LEDGER_ANCHOR_TYPE_LEGACY_GRADIDO_DB_TRANSACTION_LINK_ID:
        return LedgerAnchor::Type::LEGACY_GRADIDO_DB_TRANSACTION_LINK_ID;
      default:
        throw GradidoUnhandledEnum("error converting ledger anchor type from grdw (C-Interface)", "LedgerAnchor::Type", to_string(static_cast<uint8_t>(type)).c_str());
    }
  }

  grdw_ledger_anchor_type toGrdw(LedgerAnchor::Type type)
  {
    switch (type) {
      case LedgerAnchor::Type::UNSPECIFIED:
        return GRDW_LEDGER_ANCHOR_TYPE_UNSPECIFIED;
      case LedgerAnchor::Type::HIERO_TRANSACTION_ID:
        return GRDW_LEDGER_ANCHOR_TYPE_HIERO_TRANSACTION_ID;
      case LedgerAnchor::Type::LEGACY_GRADIDO_DB_TRANSACTION_ID:
        return GRDW_LEDGER_ANCHOR_TYPE_LEGACY_GRADIDO_DB_TRANSACTION_ID;
      case LedgerAnchor::Type::NODE_TRIGGER_TRANSACTION_ID:
        return GRDW_LEDGER_ANCHOR_TYPE_NODE_TRIGGER_TRANSACTION_ID;
      case LedgerAnchor::Type::LEGACY_GRADIDO_DB_COMMUNITY_ID:
        return GRDW_LEDGER_ANCHOR_TYPE_LEGACY_GRADIDO_DB_COMMUNITY_ID;
      case LedgerAnchor::Type::LEGACY_GRADIDO_DB_USER_ID:
        return GRDW_LEDGER_ANCHOR_TYPE_LEGACY_GRADIDO_DB_USER_ID;
      case LedgerAnchor::Type::LEGACY_GRADIDO_DB_CONTRIBUTION_ID:
        return GRDW_LEDGER_ANCHOR_TYPE_LEGACY_GRADIDO_DB_CONTRIBUTION_ID;
      case LedgerAnchor::Type::LEGACY_GRADIDO_DB_TRANSACTION_LINK_ID:
        return GRDW_LEDGER_ANCHOR_TYPE_LEGACY_GRADIDO_DB_TRANSACTION_LINK_ID;
      default:
        throw GradidoUnhandledEnum("error converting ledger anchor type to grdw (C-Interface)", "LedgerAnchor::Type", to_string(static_cast<uint8_t>(type)).c_str());
    }
  }

  BalanceDerivationType fromGrdw(grdd_balance_derivation_type type)
  {
    switch(type) {
      case GRDD_BALANCE_DERIVATION_UNSPECIFIED:
        return BalanceDerivationType::UNSPECIFIED;
      case GRDD_BALANCE_DERIVATION_NODE:
        return BalanceDerivationType::NODE;
      case GRDD_BALANCE_DERIVATION_EXTERN:
        return BalanceDerivationType::EXTERN;
      default:
        throw GradidoUnhandledEnum("error converting balance derivation type from grdw (C-Interface)", "BalanceDerivationType", to_string(static_cast<uint8_t>(type)).c_str());
    }

  }
  grdd_balance_derivation_type toGrdw(BalanceDerivationType type)
  {
    switch(type) {
      case BalanceDerivationType::UNSPECIFIED:
        return GRDD_BALANCE_DERIVATION_UNSPECIFIED;
      case BalanceDerivationType::NODE:
        return GRDD_BALANCE_DERIVATION_NODE;
      case BalanceDerivationType::EXTERN:
        return GRDD_BALANCE_DERIVATION_EXTERN;
      default:
        throw GradidoUnhandledEnum("error converting balance derivation type to grdw (C-Interface)", "BalanceDerivationType", to_string(static_cast<uint8_t>(type)).c_str());
    }
  }
}