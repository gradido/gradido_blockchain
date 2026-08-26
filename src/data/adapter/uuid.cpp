#include "gradido_blockchain/data/adapter/uuid.h"

#include "sodium.h"

#include <stdexcept>
#include <cstdio>
#include <cstring>
#include <cctype>

namespace gradido::data::adapter {

  Uuid uuidFromString(const char* uuidString)
  {
    if (!uuidString) {
      throw std::invalid_argument("UUID string is null");
    }

    // Convert the hex string (ignoring hyphens) to binary data
    const size_t hexLen = std::strlen(uuidString);
    size_t binLen = 0;
    const char* ignoreChars = "-";
    Uuid result;
    if (sodium_hex2bin((unsigned char*)result.data(), result.size(), uuidString, hexLen, ignoreChars, &binLen, nullptr) != 0) {
      throw UuidConvertException("Invalid UUID hex string", uuidString);
    }
    if (binLen != sizeof(result)) {
      throw UuidConvertException("UUID hex string does not represent 16 bytes", uuidString);
    }
    return result;
  }

  std::string uuidToString(const Uuid& uuid)
  {
    assert(uuid.size() == 16);
    char hex[33];
    sodium_bin2hex(hex, 33, uuid.data(), 16);
    // hex contains 32 characters + a null terminator
    std::string result;
    result.reserve(36);
    result.append(hex, 8); result += '-';
    result.append(hex + 8, 4); result += '-';
    result.append(hex + 12, 4); result += '-';
    result.append(hex + 16, 4); result += '-';
    result.append(hex + 20, 12);
    return result;
  }

  void uuidToString(char uuidString[37], const Uuid& uuid)
  {
    assert(uuid.size() == 16);
    char hex[33];
    sodium_bin2hex(hex, 33, uuid.data(), 16);
    int outputCursor = 0;
    const int hyphenPlaces[] = { 8, 13, 18, 23 };
    const int hyphenCount = sizeof(hyphenPlaces) / sizeof(hyphenPlaces[0]);
    int hypenIndex = 0;
    int nextHyphenPlace = hyphenPlaces[hypenIndex];
    // 48066a47-a02f-4596-883c-302c2b1aa1e1
    int i = 0;
    for (; i < 33; i++) {
      if (outputCursor == nextHyphenPlace) {
        uuidString[outputCursor++] = '-';
        hypenIndex++;
        if (hypenIndex < hyphenCount) {
          nextHyphenPlace = hyphenPlaces[hypenIndex];
        }
        else {
          break;
        }
      }
      uuidString[outputCursor++] = hex[i];
    }
    assert(i < 33);
    memcpy(&uuidString[outputCursor], &hex[i], 33 - i);
  }

  std::string UuidConvertException::getFullString() const
  {
    std::string result = what();
    result += "uuid str: " + mUuidString;
    return result;
  }
}