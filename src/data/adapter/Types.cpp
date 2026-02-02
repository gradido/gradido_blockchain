#include "gradido_blockchain/data/adapter/Types.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

#include <magic_enum/magic_enum.hpp>

using namespace magic_enum;

namespace gradido::data::adapter {
  MemoKeyType fromGrdw(grdw_memo_key_type type) 
  {
    switch(type) {
      case SHARED_SECRET:
        return MemoKeyType::SHARED_SECRET;
      case COMMUNITY_SECRET:
        return MemoKeyType::COMMUNITY_SECRET;
      case PLAIN:
        return MemoKeyType::PLAIN;
      default:
        throw GradidoUnhandledEnum("error converting memo key from grdw (C-Interface)", "MemoKeyType", enum_name(type).data());
    }
  }  
  grdw_memo_key_type toGrdw(MemoKeyType type) {
    switch(type) {
      case MemoKeyType::SHARED_SECRET:
        return SHARED_SECRET;
      case MemoKeyType::COMMUNITY_SECRET:
        return COMMUNITY_SECRET;
      case MemoKeyType::PLAIN:
        return PLAIN;
      default:
        throw GradidoUnhandledEnum("error converting memo key to grdw (C-Interface)", "MemoKeyType", enum_name(type).data());
    }
  }

  CrossGroupType fromGrdw(grdw_transaction_body_cross_group_type type)
  {
    switch(type) {
      case GRDW_TRANSACTION_BODY_CROSS_GROUP_TYPE_LOCAL:
        return CrossGroupType::LOCAL;
      case GRDW_TRANSACTION_BODY_CROSS_GROUP_TYPE_INBOUND:
        return CrossGroupType::INBOUND;
      case GRDW_TRANSACTION_BODY_CROSS_GROUP_TYPE_OUTBOUND:
        return CrossGroupType::OUTBOUND;
      case GRDW_TRANSACTION_BODY_CROSS_GROUP_TYPE_CROSS:
        return CrossGroupType::CROSS;
      default:
        throw GradidoUnhandledEnum("error converting cross group type from grdw (C-Interface)", "CrossGroupType", enum_name(type).data());
    }
  }
  grdw_transaction_body_cross_group_type toGrdw(CrossGroupType type)
  {
    switch(type) {
      case CrossGroupType::LOCAL:
        return GRDW_TRANSACTION_BODY_CROSS_GROUP_TYPE_LOCAL;
      case CrossGroupType::INBOUND:
        return GRDW_TRANSACTION_BODY_CROSS_GROUP_TYPE_INBOUND;
      case CrossGroupType::OUTBOUND:
        return GRDW_TRANSACTION_BODY_CROSS_GROUP_TYPE_OUTBOUND;
      case CrossGroupType::CROSS:
        return GRDW_TRANSACTION_BODY_CROSS_GROUP_TYPE_CROSS;
      default:
        throw GradidoUnhandledEnum("error converting cross group type to grdw (C-Interface)", "CrossGroupType", enum_name(type).data());
    }
  }
  
  TransactionType fromGrdw(grdw_transaction_type type) 
  {
    switch(type) {
      case GRDW_TRANSACTION_TYPE_NONE:
        return TransactionType::NONE;
      case GRDW_TRANSACTION_TYPE_CREATION:
        return TransactionType::CREATION;
      case GRDW_TRANSACTION_TYPE_TRANSFER:
        return TransactionType::TRANSFER;
      case GRDW_TRANSACTION_TYPE_COMMUNITY_FRIENDS_UPDATE:
        return TransactionType::COMMUNITY_FRIENDS_UPDATE;
      case GRDW_TRANSACTION_TYPE_REGISTER_ADDRESS:
        return TransactionType::REGISTER_ADDRESS;
      case GRDW_TRANSACTION_TYPE_DEFERRED_TRANSFER:
        return TransactionType::DEFERRED_TRANSFER;
      case GRDW_TRANSACTION_TYPE_COMMUNITY_ROOT:
        return TransactionType::COMMUNITY_ROOT;
      case GRDW_TRANSACTION_TYPE_REDEEM_DEFERRED_TRANSFER:
        return TransactionType::REDEEM_DEFERRED_TRANSFER;
      case GRDW_TRANSACTION_TYPE_TIMEOUT_DEFERRED_TRANSFER:
        return TransactionType::TIMEOUT_DEFERRED_TRANSFER;
      default:
        throw GradidoUnhandledEnum("error converting transaction type from grdw (C-Interface)", "TransactionType", enum_name(type).data());
    }
  }
  grdw_transaction_type toGrdw(TransactionType type)
  {
    switch(type) {
      case TransactionType::NONE:
        return GRDW_TRANSACTION_TYPE_NONE;
      case TransactionType::CREATION:
        return GRDW_TRANSACTION_TYPE_CREATION;
      case TransactionType::TRANSFER:
        return GRDW_TRANSACTION_TYPE_TRANSFER;
      case TransactionType::COMMUNITY_FRIENDS_UPDATE:
        return GRDW_TRANSACTION_TYPE_COMMUNITY_FRIENDS_UPDATE;
      case TransactionType::REGISTER_ADDRESS:
        return GRDW_TRANSACTION_TYPE_REGISTER_ADDRESS;
      case TransactionType::DEFERRED_TRANSFER:
        return GRDW_TRANSACTION_TYPE_DEFERRED_TRANSFER;
      case TransactionType::COMMUNITY_ROOT:
        return GRDW_TRANSACTION_TYPE_COMMUNITY_ROOT;
      case TransactionType::REDEEM_DEFERRED_TRANSFER:
        return GRDW_TRANSACTION_TYPE_REDEEM_DEFERRED_TRANSFER;
      case TransactionType::TIMEOUT_DEFERRED_TRANSFER:
        return GRDW_TRANSACTION_TYPE_TIMEOUT_DEFERRED_TRANSFER;
      default:
        throw GradidoUnhandledEnum("error converting transaction type to grdw (C-Interface)", "TransactionType", enum_name(type).data());
    }
  }
  
  AddressType fromGrdw(grdw_address_type type)
  {
    switch(type) {
      case GRDW_ADDRESS_TYPE_NONE:
        return AddressType::NONE;
      case GRDW_ADDRESS_TYPE_COMMUNITY_HUMAN:
        return AddressType::COMMUNITY_HUMAN;
      case GRDW_ADDRESS_TYPE_COMMUNITY_GMW:
        return AddressType::COMMUNITY_GMW;
      case GRDW_ADDRESS_TYPE_COMMUNITY_AUF:
        return AddressType::COMMUNITY_AUF;
      case GRDW_ADDRESS_TYPE_COMMUNITY_PROJECT:
        return AddressType::COMMUNITY_PROJECT;
      case GRDW_ADDRESS_TYPE_SUBACCOUNT:
        return AddressType::SUBACCOUNT;
      case GRDW_ADDRESS_TYPE_CRYPTO_ACCOUNT:
        return AddressType::CRYPTO_ACCOUNT;
      default:
        throw GradidoUnhandledEnum("error converting address type from grdw (C-Interface)", "AddressType", enum_name(type).data());
    }
  }
  grdw_address_type toGrdw(AddressType type)
  {
    switch(type) {
      case AddressType::NONE:
        return GRDW_ADDRESS_TYPE_NONE;
      case AddressType::COMMUNITY_HUMAN:
        return GRDW_ADDRESS_TYPE_COMMUNITY_HUMAN;
      case AddressType::COMMUNITY_GMW:
        return GRDW_ADDRESS_TYPE_COMMUNITY_GMW;
      case AddressType::COMMUNITY_AUF:
        return GRDW_ADDRESS_TYPE_COMMUNITY_AUF;
      case AddressType::COMMUNITY_PROJECT:
        return GRDW_ADDRESS_TYPE_COMMUNITY_PROJECT;
      case AddressType::SUBACCOUNT:
        return GRDW_ADDRESS_TYPE_SUBACCOUNT;
      case AddressType::CRYPTO_ACCOUNT:
        return GRDW_ADDRESS_TYPE_CRYPTO_ACCOUNT;
      default:
        throw GradidoUnhandledEnum("error converting address type to grdw (C-Interface)", "AddressType", enum_name(type).data());
    }
  }
}