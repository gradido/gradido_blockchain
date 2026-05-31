#include "gradido_blockchain/data/runtime/CompleteTransaction.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/interaction/deserialize/Context.h"
#include "gradido_blockchain/memory/grdu_StaticBuffer.h"
#include "gradido_blockchain_core/data/wire/confirmed_transaction.h"
#include "gradido_blockchain_core/data/wire/transaction_body.h"
#include "gradido_blockchain_core/mapping/runtime_from_wire.h"
#include "gradido_blockchain_core/memory.h"
#include "gradido_blockchain_core/result.h"

#include "magic_enum/magic_enum.hpp"

#include <vector>

using namespace magic_enum;
using std::vector;

namespace gradido::data::runtime {
  CompleteTransaction::CompleteTransaction()
  {
    grdr_complete_transaction_init(this);
  }

  CompleteTransaction::~CompleteTransaction()
  {
    grdr_complete_transaction_release(this);
  }

  grd_result CompleteTransaction::initFromGrdw(
    const grdw_transaction_body* body,
    const grdw_confirmed_transaction* confirmedTx,
    Uuid communityUuid
  )
  {
    grdr_complete_transaction_release(this);
    return grdm_complete_transaction_from_wire(this, body, confirmedTx, communityUuid.data());    
  }

  grd_result CompleteTransaction::initFromProtobuf(const grd_memory_block& inputBuffer, Uuid communityUuid)
  {
    memory::GrduStaticBuffer<4096> buffer;
    return buffer.use(
      [&](grd_memory* alloc) -> grd_result
      {
        grdw_confirmed_transaction tx;
        grdw_confirmed_transaction_init(&tx);
        auto result = grdw_confirmed_transaction_decode(&tx, &inputBuffer, alloc);
        // we skip GRD_ERROR_OUT_OF_MEMORY because GrduStaticBuffer should handle this error
        if (GRD_SUCCESS != result && GRD_ERROR_OUT_OF_MEMORY != result) {
          LOG_F(ERROR, "decode error: %s", enum_name(result).data());
          throw GradidoNodeInvalidDataException("error deserialize confirmed transaction");
        }
        if (GRD_SUCCESS != result) { return result; }

        grdw_transaction_body body;
        grdw_transaction_body_init(&body);
        result = grdw_transaction_body_decode(&body, &tx.transaction.body_bytes, alloc);
        // we skip GRD_ERROR_STATIC_BUFFER_TO_SMALL because GrduStaticBuffer should handle this error
        if (GRD_SUCCESS != result && GRD_ERROR_OUT_OF_MEMORY != result) {
          LOG_F(ERROR, "body decode error: %s", enum_name(result).data());
          throw GradidoNodeInvalidDataException("error deserialize transaction body");
        }
        if (GRD_SUCCESS != result) { return result; }
        return initFromGrdw(&body, &tx, communityUuid);
      }
    );
  }

  vector<grdw_account_balance> CompleteTransaction::getAccountBalances() const
  {
    if (!account_balances_count) {
      return {};
    }
    vector<grdw_account_balance> result;
    result.reserve(account_balances_count);
    for (int i = 0; i < account_balances_count; ++i) {
      result.push_back(account_balances[i]);
    }
    return result;
  }

  bool CompleteTransaction::hasAccountBalance(const PublicKey& publicKey, const Uuid& coinCommunityUuid) const
  {
    if (!account_balances_count) {
      return false;
    }
    for (int i = 0; i < account_balances_count; ++i) {
      if (publicKey.isTheSame(account_balances[i].pubkey) && coinCommunityUuid.isTheSame(account_balances[i].community_uuid)) {
        return true;
      }
    }
    return false;
  }

  grdw_account_balance CompleteTransaction::getAccountBalance(const PublicKey& publicKey, const Uuid& coinCommunityUuid) const
  {
    if (!account_balances_count) {
      return {};
    }
    for (int i = 0; i < account_balances_count; ++i) {
      if (publicKey.isTheSame(account_balances[i].pubkey) && coinCommunityUuid.isTheSame(account_balances[i].community_uuid)) {
        return account_balances[i];
      }
    }
    return {};
  }

  bool CompleteTransaction::isInvolved(const PublicKey& publicKey) const
  {
    if (isBalanceUpdated(publicKey)) { return true; }
    if (signature_pairs_count) {
      for (int i = 0; i < signature_pairs_count; ++i) {
        if (publicKey.isTheSame(signature_pairs[i].public_key)) {
          return true;
        }
      }
    }
    return false;
  }

  bool CompleteTransaction::isBalanceUpdated(const PublicKey& publicKey) const
  {
    if (!account_balances_count) {
      return false;
    }
    for (int i = 0; i < account_balances_count; ++i) {
      if (publicKey.isTheSame(account_balances[i].pubkey)) {
        return true;
      }
    }
    return false;
  }

  vector<PublicKey> CompleteTransaction::getInvolvedAddresses() const
  {
    vector<PublicKey> result;
    switch (transaction_type) {
    case GRDT_TRANSACTION_TRANSFER:
    case GRDT_TRANSACTION_DEFERRED_TRANSFER:
    case GRDT_TRANSACTION_REDEEM_DEFERRED_TRANSFER:
    case GRDT_TRANSACTION_TIMEOUT_DEFERRED_TRANSFER:
      for (int i = 0; i < account_balances_count; ++i) {
        result.push_back(account_balances[i].pubkey);
      }
      break;

    case GRDT_TRANSACTION_CREATION:
      result.push_back(transfer.recipient_pubkey);
      if (signature_pairs_count != 1) {
        throw GradidoNodeInvalidDataException("other signatures count in creation as expected");
      }
      result.push_back(signature_pairs[0].public_key);
      break;

    case GRDT_TRANSACTION_REGISTER_ADDRESS:
    case GRDT_TRANSACTION_COMMUNITY_FRIENDS_UPDATE:
      for (int i = 0; i < signature_pairs_count; ++i) {
        result.push_back(signature_pairs[i].public_key);
      }
      break;

    case GRDT_TRANSACTION_COMMUNITY_ROOT:
      result.push_back(community_root.public_key);
      result.push_back(community_root.gmw_public_key);
      result.push_back(community_root.auf_public_key);
      break;
    default: 
      throw GradidoUnhandledEnum("on CompleteTransaction::getInvolvedAddresses", "grdt_transaction", enum_name(transaction_type).data());
    }
    return result;
  }

  vector<grdw_signature_pair> CompleteTransaction::getSignatureMap()
  {
    if (!signature_pairs_count) {
      return {};
    }
    vector<grdw_signature_pair> signatures;
    for (int i = 0; i < signature_pairs_count; ++i) {
      signatures.push_back(signature_pairs[i]);
    }
    return signatures;
  }

  vector<EncryptedMemo> CompleteTransaction::getMemos() const
  {
    vector<EncryptedMemo> memos;
    if (!encrypted_memos_count) {
      return {};
    }
    for (int i = 0; i < encrypted_memos_count; ++i) {
      memos.push_back(encrypted_memos[i]);
    }
    return memos;    
  }
}