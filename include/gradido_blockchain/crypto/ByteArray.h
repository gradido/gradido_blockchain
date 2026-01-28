#ifndef __GRADIDO_BLOCKCHAIN_CRYPTO_PUBLIC_KEY_H
#define __GRADIDO_BLOCKCHAIN_CRYPTO_PUBLIC_KEY_H

#include "SignatureOctet.h"
#include "gradido_blockchain/export.h"
#include "gradido_blockchain/types.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

#include "sodium.h"

#include <cstddef>
#include <span>
#include <vector>

template<std::size_t N>
class ByteArray
{
public:
  ByteArray(std::span<std::byte> data)
  {
    if (data.size() != N) {
      throw InvalidSizeException("ByteArray constructor called with wrong data", N, data.size());
    }
    memcpy(mData, data.data(), N);
  }
  ByteArray(std::vector<uint8_t> data)
  {
    if (data.size() != N) {
      throw InvalidSizeException("ByteArray constructor called with wrong data", N, data.size());
    }
    memcpy(mData, data.data(), N);
  }

  ByteArray(const uint8_t data[N]) { memcpy(mData, data, N); }

  inline const uint8_t* data() const { return mData; }
  inline size_t size() const { return N; }
  inline bool operator==(const ByteArray& other) const { return std::memcmp(mData, other.mData, N) == 0; }
  inline bool operator!=(const ByteArray& other) const { return !(*this == other); }
  inline bool isTheSame(const ByteArray& other) const { return std::memcmp(mData, other.mData, N) == 0; }
  inline bool isTheSame(uint8_t* data) const { return std::memcmp(mData, data, N) == 0; }
  inline bool isEmpty() const;
  inline std::vector<uint8_t> copyAsVector() const { return { mData, mData + N }; }
  inline std::string convertToHex() const;
    
protected:
  uint8_t mData[N];
};

template<std::size_t N>
inline bool ByteArray<N>::isEmpty() const {
  for (auto i = 0; i < N; i++) {
    if (mData[i] != 0) {
      return false;
    }
  }
  return true;
}

template<std::size_t N>
inline std::string ByteArray<N>::convertToHex() const {
  constexpr uint32_t hexSize = N * 2 + 1;
  char buffer[hexSize];
  sodium_bin2hex(buffer, hexSize, mData, N);
  return { buffer, hexSize - 1 };
}

template<std::size_t N>
struct ByteArrayHash
{
  size_t operator()(const ByteArray<N>& s) const noexcept {
    return SignatureOctet(s.data(), N).octet;
  }
};

template<std::size_t N>
struct ByteArrayEqual
{
  bool operator()(const ByteArray<N>& a, const ByteArray<N>& b) const noexcept {
    return a.isTheSame(b);
  }
};

typedef ByteArray<32> PublicKey;
typedef ByteArrayHash<32> PublicKeyHash;
typedef ByteArrayEqual<32> PublicKeyEqual;
typedef ByteArray<32> GenericHash;
typedef ByteArrayHash<32> GenericHashHash;
typedef ByteArrayEqual<32> GenericHashEqual;
typedef ByteArray<64> Signature;
typedef ByteArray<64> PrivateKey;

#endif // __GRADIDO_BLOCKCHAIN_CRYPTO_PUBLIC_KEY_H