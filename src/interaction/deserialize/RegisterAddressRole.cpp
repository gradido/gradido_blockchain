#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/crypto/ByteArray.h"
#include "gradido_blockchain/data/AddressType.h"
#include "gradido_blockchain/data/compact/PublicKeyIndex.h"
#include "gradido_blockchain/data/compact/RegisterAddressTx.h"
#include "gradido_blockchain/interaction/deserialize/RegisterAddressRole.h"
#include "gradido_blockchain/interaction/deserialize/Exceptions.h"

#include <memory>

using memory::Block;
using std::make_unique;

namespace gradido {
  using data::AddressType;
  using data::compact::PublicKeyIndex, data::compact::RegisterAddressTx;

  namespace interaction::deserialize {

    RegisterAddressRole::RegisterAddressRole(const RegisterAddressMessage& registerAddressMessage)
        : mRegisterAddressMessage(registerAddressMessage) 
    {

    }

    RegisterAddressTx RegisterAddressRole::run(uint32_t communityIdIndex) const
    {
      if (!mRegisterAddressMessage["address_type"_f].has_value()) {
        throw MissingMemberException("missing member on deserialize register address transaction", "address_type");
      }
      if (!mRegisterAddressMessage["derivation_index"_f].has_value()) {
	      throw MissingMemberException("missing member on deserialize register address transaction", "derivation_index");
      }
      if (!mRegisterAddressMessage["user_pubkey"_f].has_value()) {
        throw MissingMemberException("missing member on deserialize register address transaction", "user_pubkey");
      }
      if (!mRegisterAddressMessage["name_hash"_f].has_value()) {
        throw MissingMemberException("missing member on deserialize register address transaction", "name_hash");
      }
      if (!mRegisterAddressMessage["account_pubkey"_f].has_value()) {
        throw MissingMemberException("missing member on deserialize register address transaction", "account_pubkey");
      }

      PublicKey userPubkey(mRegisterAddressMessage["user_pubkey"_f].value());
      if (userPubkey.isEmpty()) {
        throw EmptyMemberException("empty member on deserialize register address transaction", "user_pubkey");
      }
      GenericHash nameHash(mRegisterAddressMessage["name_hash"_f].value());
      if (nameHash.isEmpty()) {
        throw EmptyMemberException("empty member on deserialize register address transaction", "name_hash");
      }
      PublicKey accountPubkey(mRegisterAddressMessage["account_pubkey"_f].value());
      if (accountPubkey.isEmpty()) {
        throw EmptyMemberException("empty member on deserialize register address transaction", "account_pubkey");
      }
      RegisterAddressTx result;

      return {
          .addressType = mRegisterAddressMessage["address_type"_f].value(),
          .derivationIndex = static_cast<uint16_t>(mRegisterAddressMessage["derivation_index"_f].value()),
          .nameHashIndex = static_cast<uint32_t>(g_appContext->getOrAddUserNameHashIndex(nameHash)),
          .userPublicKeyIndex = {
            .communityIdIndex = communityIdIndex,
            .publicKeyIndex = g_appContext->getOrAddPublicKeyIndex(communityIdIndex, userPubkey)
          },
          .accountPublicKeyIndex = {
            .communityIdIndex = communityIdIndex,
            .publicKeyIndex = g_appContext->getOrAddPublicKeyIndex(communityIdIndex, accountPubkey)
          }
      };    
    }
  }
}