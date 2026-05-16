#include "gradido_blockchain/lib/Uuid.h"

#include "sodium.h"

#include <stdexcept>
#include <cstdio>
#include <cstring>
#include <cctype>

Uuid::Uuid()
{
  memset(mUuid, 0, 16);
}

Uuid::Uuid(const char* uuidString)
{
  if (!uuidString) {
    throw std::invalid_argument("UUID string is null");
  }

  // Konvertiere den Hex-String (ignoriere Bindestriche) in binäre Daten
  const size_t hexLen = std::strlen(uuidString);
  size_t binLen = 0;
  const char* ignoreChars = "-";

  if (sodium_hex2bin(mUuid, sizeof(mUuid), uuidString, hexLen, ignoreChars, &binLen, nullptr) != 0) {
    throw std::invalid_argument("Invalid UUID hex string");
  }
  if (binLen != sizeof(mUuid)) {
    throw std::invalid_argument("UUID hex string does not represent 16 bytes");
  }
}

Uuid::Uuid(const uint8_t uuid[16])
{
  std::memcpy(mUuid, uuid, 16);
}

std::string Uuid::toString() const {
  char hex[33];
  sodium_bin2hex(hex, 33, mUuid, 16);
  // hex enthält 32 Zeichen + Nullterminator
  std::string result;
  result.reserve(36);
  result.append(hex, 8); result += '-';
  result.append(hex + 8, 4); result += '-';
  result.append(hex + 12, 4); result += '-';
  result.append(hex + 16, 4); result += '-';
  result.append(hex + 20, 12);
  return result;
}
