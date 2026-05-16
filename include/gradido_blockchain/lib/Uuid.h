#ifndef GRADIDO_BLOCKCHAIN_LIB_UUID_H
#define GRADIDO_BLOCKCHAIN_LIB_UUID_H

#include "gradido_blockchain/export.h"
#include <string>
#include <cstdint>

class GRADIDOBLOCKCHAIN_EXPORT Uuid
{
public:
  Uuid();
  explicit Uuid(const char* uuidString);
  explicit Uuid(const uint8_t uuid[16]); // const!
  Uuid(const Uuid&) = default;
  Uuid(Uuid&&) = default;
  ~Uuid() = default;

  Uuid& operator=(const Uuid&) = default;
  Uuid& operator=(Uuid&&) = default;

  std::string toString() const;
  const uint8_t* data() const { return mUuid; }

private:
  uint8_t mUuid[16];
};

#endif