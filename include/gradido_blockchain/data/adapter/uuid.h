#ifndef GRADIDO_BLOCKCHAIN_LIB_UUID_H
#define GRADIDO_BLOCKCHAIN_LIB_UUID_H

#include "gradido_blockchain/data/ByteArray.h"
#include "gradido_blockchain/export.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include <string>
#include <cstdint>

namespace gradido::data::adapter {

  GRADIDOBLOCKCHAIN_EXPORT Uuid uuidFromString(const char* uuidString);
  GRADIDOBLOCKCHAIN_EXPORT std::string uuidToString(const Uuid& uuid);
  GRADIDOBLOCKCHAIN_EXPORT void uuidToString(char uuidString[37], const Uuid& uuid);

  class GRADIDOBLOCKCHAIN_EXPORT UuidConvertException : public GradidoBlockchainException
  {
  public:
    explicit UuidConvertException(const char* what, const char* uuidString) noexcept : GradidoBlockchainException(what), mUuidString(uuidString) {}
    virtual ~UuidConvertException() {};
    virtual std::string getFullString() const;

  protected:
    std::string mUuidString;
  };

}

#endif