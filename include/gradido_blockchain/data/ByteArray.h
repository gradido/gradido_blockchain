#ifndef __GRADIDO_BLOCKCHAIN_CRYPTO_PUBLIC_KEY_H
#define __GRADIDO_BLOCKCHAIN_CRYPTO_PUBLIC_KEY_H

#include "gradido_blockchain/crypto/SignatureOctet.h"
#include "gradido_blockchain/export.h"
#include "gradido_blockchain/types.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

#include "sodium.h"

#include <cstddef>
#include <cstring>
#include <span>
#include <vector>

namespace gradido::data {
  template<std::size_t N>
  class ByteArray
  {
  public:
    using ConstViewType = std::span<const uint8_t, N>;

    ByteArray()
    {
      memset(mData, 0, N);
    }
    ByteArray(std::span<std::byte> data)
    {
      if (data.size() != N) {
        throw InvalidSizeException("ByteArray constructor called with wrong data", N, data.size());
      }
      memcpy(mData, data.data(), N);
    }

    ByteArray(std::span<const uint8_t, N> data)
    {
      memcpy(mData, data.data(), N);
    }
    ByteArray(std::vector<uint8_t> data)
    {
      if (data.size() != N) {
        throw InvalidSizeException("ByteArray constructor called with wrong data", N, data.size());
      }
      memcpy(mData, data.data(), N);
    }
    // copy
    ByteArray(const ByteArray<N>& other)
    {
        memcpy(mData, other.mData, N);
    }
    // move
    ByteArray(ByteArray<N>&& other)
    {
        memcpy(mData, other.mData, N);
        memset(other.mData, 0, N);
    }

    // copy assignment
    ByteArray& operator=(const ByteArray<N>& other)
    {
        if (this != &other) {
            memcpy(mData, other.mData, N);
        }
        return *this;
    }

    // move assignment
    ByteArray& operator=(ByteArray<N>&& other)
    {
        if (this != &other) {
            memcpy(mData, other.mData, N);
            memset(other.mData, 0, N);
        }
        return *this;
    }

    ByteArray(const uint8_t data[N]) { memcpy(mData, data, N); }

    inline operator ConstViewType() const { return ConstViewType(mData, N); }
    inline ConstViewType view() const { return ConstViewType(mData, N); }

    inline const uint8_t* data() const { return mData; }
    inline size_t size() const { return N; }

    inline bool operator==(const ByteArray& other) const { return std::memcmp(mData, other.mData, N) == 0; }
    inline bool operator!=(const ByteArray& other) const { return !(*this == other); }
    inline bool operator<(const ByteArray& other) const { return std::memcmp(mData, other.mData, N) < 0; }
    inline bool isTheSame(const ByteArray& other) const { return std::memcmp(mData, other.mData, N) == 0; }
    inline bool isTheSame(uint8_t* data) const { return std::memcmp(mData, data, N) == 0; }
    inline bool isTheSame(ConstViewType other) const { return std::memcmp(mData, other.data(), N) == 0; }
    inline bool isEmpty() const;
    inline std::vector<uint8_t> copyAsVector() const { return { mData, mData + N }; }
    inline std::string convertToHex() const;
    static inline ByteArray fromHex(const char* hexString, size_t stringSize);

  protected:
    uint8_t mData[N];
  };

  template<std::size_t N>
  bool ByteArray<N>::isEmpty() const {
    for (auto i = 0; i < N; i++) {
      if (mData[i] != 0) {
        return false;
      }
    }
    return true;
  }

  template<std::size_t N>
  std::string ByteArray<N>::convertToHex() const {
    constexpr uint32_t hexSize = N * 2 + 1;
    char buffer[hexSize];
    sodium_bin2hex(buffer, hexSize, mData, N);
    return { buffer, hexSize - 1 };
  }

  template<std::size_t N>
  ByteArray<N> ByteArray<N>::fromHex(const char* hexString, size_t stringSize)
  {
    size_t binSize = (stringSize) / 2;
    // invalid hex size
    if (binSize * 2 != stringSize) {
      throw GradidoInvalidHexException("invalid hex size Block::fromHex", hexString);
    }
    if (binSize != N) {
      throw GradidoInvalidHexException("invalid size Parameter for ByteArray", hexString);
    }
    ByteArray<N> result;

    size_t resultBinSize = 0;
    if (0 != sodium_hex2bin(result.mData, binSize, hexString, stringSize, nullptr, &resultBinSize, nullptr)) {
      throw GradidoInvalidHexException("invalid hex for Block::fromHex", hexString);
    }
    return result;
  }

  template<size_t N>
  inline bool isTheSame(std::span<const uint8_t, N> a, std::span<const uint8_t, N> b) {
    return std::memcmp(a.data(), b.data(), N) == 0;
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
  typedef ByteArray<crypto_generichash_BYTES> GenericHash;
  typedef ByteArrayHash<crypto_generichash_BYTES> GenericHashHash;
  typedef ByteArrayEqual<crypto_generichash_BYTES> GenericHashEqual;
  typedef ByteArray<16> Uuid;
  typedef ByteArrayHash<16> UuidHash;
  typedef ByteArrayEqual<16> UuidEqual;
  typedef ByteArray<64> Signature;
  typedef ByteArray<64> PrivateKey;

}
#endif // __GRADIDO_BLOCKCHAIN_CRYPTO_PUBLIC_KEY_H
