#ifndef __GRADIDO_BLOCKCHAIN_DATA_COMPACT_CONFIRMED_GRADIDO_TX_H__
#define __GRADIDO_BLOCKCHAIN_DATA_COMPACT_CONFIRMED_GRADIDO_TX_H__  

#include "AccountBalance.h"
#include "CommunityRootTx.h"
#include "CreationTx.h"
#include "DeferredTransferTx.h"
#include "PublicKeyIndex.h"
#include "RegisterAddressTx.h"
#include "TransferTx.h"
#include "TxId.h"
#include "gradido_blockchain/export.h"
#include "gradido_blockchain/data/AddressType.h"
#include "gradido_blockchain/data/BalanceDerivationType.h"
#include "gradido_blockchain/data/CrossGroupType.h"
#include "gradido_blockchain/data/rich/AccountBalance.h"
#include "gradido_blockchain/data/Timestamp.h"
#include "gradido_blockchain/data/TransactionType.h"
#include "gradido_blockchain/GradidoUnit.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/lib/DictionaryInterface.h"

#include <date/date.h>
#include <memory>
#include <string>
#include <vector>

struct grdw_confirmed_transaction;
struct grdw_gradido_transaction;
struct grdw_transaction_body;

namespace gradido {
  class AppContext;
  namespace data::compact {
    struct ConfirmedGradidoTxCold;

    struct GRADIDOBLOCKCHAIN_EXPORT ConfirmedGradidoTx
    {
      ConfirmedGradidoTx();
      ~ConfirmedGradidoTx();

      // copy constructor
      ConfirmedGradidoTx(const ConfirmedGradidoTx& other);
      // move
      ConfirmedGradidoTx(ConfirmedGradidoTx&& other)  noexcept;

      // also move
      ConfirmedGradidoTx& operator=(ConfirmedGradidoTx&& other)  noexcept;
      // also copy
      ConfirmedGradidoTx& operator=(const ConfirmedGradidoTx& other);

      static ConfirmedGradidoTx fromGrdw(
        const grdw_confirmed_transaction* tx,
        uint32_t blockchainCommunityIdIndex,
        AppContext& appContext,
        bool loadColdData = true
      );

      static ConfirmedGradidoTx fromGrdw(
        const grdw_confirmed_transaction* tx,
        const grdw_transaction_body* body,
        uint32_t blockchainCommunityIdIndex,
        AppContext& appContext,
        bool loadColdData = true
      );

      static ConfirmedGradidoTx fromGrdw(
        const grdw_gradido_transaction* tx,
        uint32_t blockchainCommunityIdIndex,
        AppContext& appContext,
        bool loadColdData = true
      );

      static ConfirmedGradidoTx fromGrdw(
        const grdw_gradido_transaction* tx,
        const grdw_transaction_body* body,
        uint32_t blockchainCommunityIdIndex,
        AppContext& appContext,
        bool loadColdData = true
      );

      void fillFromGrdwTransactionBody(const grdw_transaction_body* body, AppContext& appContext);

      // packed tx and timestamp together to save 8 Byte padding
      uint64_t txNr;
      uint64_t confirmedAtSeconds;
      uint32_t confirmedAtNanos;
      uint32_t txCommunityIdIndex;
      uint32_t coinCommunityIdIndex;

      inline Timestamp getConfirmedAt() const { return Timestamp(confirmedAtSeconds, confirmedAtNanos); }
      inline TxId getTxId() const { return TxId(txNr, txCommunityIdIndex); }

      // enums, usually uint8_t
      CrossGroupType crossGroupType;
      TransactionType transactionType;
      BalanceDerivationType balanceDerivationType;
      uint8_t accountBalanceCount;

      // TODO: check if bring more performance to put this else where
      AccountBalance accountBalances[3];

      // common fields for most transactions
      union { // 16 Bytes
        CreationTx creation;
        TransferTx transfer;
        DeferredTransferTx deferredTransfer; // fund deferred transfer address only on your own community
        RegisterAddressTx registerAddress;
        CommunityRootTx communityRoot;
      } specific;
      // cold data, not on hot path, should be used less frequently
      // don't always exist!
      std::unique_ptr<ConfirmedGradidoTxCold> coldData;

      // if cold isn't loaded, doesn't contain pubkeys from signature map
      std::vector<PublicKeyIndex> getInvolvedAddresses() const;
      //! \return all public key indices belongs to the same community as txCommunityId
      std::vector<uint32_t> getBalanceChangingPublicKeyIndices() const;
      //! \return work only if cold data exist, returned public key indices belong to txCommunityId
      std::vector<uint32_t> getSignaturePublicKeyIndices(const IDictionary<PublicKey>& publicKeyDict) const;
      bool isSignaturePublicKey(PublicKeyIndex pubkexIndex, const IDictionary<PublicKey>& publicKeyDict) const;
      //! \return full public key indices for addresses not signing, not account balance changing but involved in tx, maybe from another community
      std::vector<PublicKeyIndex> getOtherInvolved() const;
      bool isOtherInvolved(PublicKeyIndex pubkeyIndex) const;
      bool isBalanceUpdated(PublicKeyIndex pubkeyIndex) const;
      // if cold isn't loaded, doesn't check pubkeys from signature map
      bool isInvolved(PublicKeyIndex pubkeyIndex) const;

      inline bool isTransfer() const { return TransactionType::TRANSFER == transactionType; }
      inline bool isCreation() const { return TransactionType::CREATION == transactionType; }
      inline bool isRegisterAddress() const { return TransactionType::REGISTER_ADDRESS == transactionType; }
      inline bool isDeferredTransfer() const { return TransactionType::DEFERRED_TRANSFER == transactionType; }
      inline bool isRedeemDeferredTransfer() const { return TransactionType::REDEEM_DEFERRED_TRANSFER == transactionType; }
      inline bool isTimeoutDeferredTransfer() const { return TransactionType::TIMEOUT_DEFERRED_TRANSFER == transactionType; }
      inline bool isCommunityRoot() const { return TransactionType::COMMUNITY_ROOT == transactionType; }
      inline bool isCrossCommunityTx() const { return CrossGroupType::LOCAL != crossGroupType; }
      inline bool isOutboundCommunityTx() const { return CrossGroupType::OUTBOUND == crossGroupType; }
      inline bool isInboundCrossCommunityTx() const { return CrossGroupType::INBOUND == crossGroupType; }
      // can be also used for not confirmed transactions
      inline bool isConfirmedTx() const { return txNr != 0; }

      bool hasCoinsFromCommunity(uint32_t coinColorCommunityId) const;
      inline bool hasColdData() const { return (bool)coldData; }

      // full public key indices
      //! get sender public key index if it transfer or deferred transfer transaction else std::nullopt
      PublicKeyIndex getSender() const;
      //! get recipient public key index if it is creation, transfer or deferred transfer transaction else std::nullopt
      PublicKeyIndex getRecipient() const;
      //! get user public key on register address transaction else std::nullopt
      inline PublicKeyIndex getRegisteredUser() const;
      //! get account public key on register address transaction else std::nullopt
      inline PublicKeyIndex getRegisteredAccount() const;
      //! get root public key on community root transaction else std::nullopt
      inline PublicKeyIndex getCommunityRootPublicKey() const;
      //! get auf account public key on community root transaction else std::nullopt
      inline PublicKeyIndex getAuf() const;
      //! get gmw account public key on community root transaction else std::nullopt
      inline PublicKeyIndex getGmw() const;

      rich::AccountBalance getAccountBalance(PublicKeyIndex publicKeyIndex, uint32_t coinCommunityIdIndex) const;
      inline GradidoUnit getAmount() const;
    };

    //! get user public key on register address transaction else empty PublicKeyIndex
    PublicKeyIndex ConfirmedGradidoTx::getRegisteredUser() const
    {
      if (!isRegisterAddress()) return PublicKeyIndex();
      return PublicKeyIndex{
        .communityIdIndex = txCommunityIdIndex,
        .publicKeyIndex = specific.registerAddress.userPublicKeyIndex
      };
    }
    //! get account public key on register address transaction else std::nullopt
    PublicKeyIndex ConfirmedGradidoTx::getRegisteredAccount() const
    {
      if (!isRegisterAddress()) return PublicKeyIndex();
      return PublicKeyIndex{
        .communityIdIndex = txCommunityIdIndex,
        .publicKeyIndex = specific.registerAddress.accountPublicKeyIndex
      };
    }

    PublicKeyIndex ConfirmedGradidoTx::getCommunityRootPublicKey() const
    {
      if (!isCommunityRoot()) return PublicKeyIndex();
      return PublicKeyIndex{
       .communityIdIndex = txCommunityIdIndex,
       .publicKeyIndex = specific.communityRoot.publicKeyIndex
      };
    }

    PublicKeyIndex ConfirmedGradidoTx::getAuf() const
    {
      if (!isCommunityRoot()) return PublicKeyIndex();
      return PublicKeyIndex{
       .communityIdIndex = txCommunityIdIndex,
       .publicKeyIndex = specific.communityRoot.aufPublicKeyIndex
      };
    }

    PublicKeyIndex ConfirmedGradidoTx::getGmw() const
    {
      if (!isCommunityRoot()) return PublicKeyIndex();
      return PublicKeyIndex{
       .communityIdIndex = txCommunityIdIndex,
       .publicKeyIndex = specific.communityRoot.gmwPublicKeyIndex
      };
    }

    GradidoUnit ConfirmedGradidoTx::getAmount() const
    {
      if (isTransfer() || isRedeemDeferredTransfer()) {
        return GradidoUnit::fromGradidoCent(specific.transfer.amountGddCent);
      }
      else if (isDeferredTransfer()) {
        return GradidoUnit::fromGradidoCent(specific.deferredTransfer.amountGddCent);
      }
      else if (isCreation()) {
        return GradidoUnit::fromGradidoCent(specific.creation.amountGddCent);
      }
      return GradidoUnit::zero();

    }

    using ConstConfirmedTxPtr = std::shared_ptr<const ConfirmedGradidoTx>;
    using ConfirmedTxs = std::vector<ConstConfirmedTxPtr>;
    
    //  Exceptions

    class MissingColdDataException : public GradidoBlockchainException
    {
    public:
      explicit MissingColdDataException(const char* what, const ConfirmedGradidoTx& parent) noexcept;
      std::string getFullString() const;
    protected:
      uint64_t mTxNr;
      data::TransactionType mTransactionType;
    };
  }
}

#endif // __GRADIDO_BLOCKCHAIN_DATA_COMPACT_CONFIRMED_GRADIDO_TX_H__