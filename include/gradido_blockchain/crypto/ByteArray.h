#ifndef __GRADIDO_BLOCKCHAIN_CRYPTO_PUBLIC_KEY_H
#define __GRADIDO_BLOCKCHAIN_CRYPTO_PUBLIC_KEY_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/types.h"

template<std::size_t N>
class ByteArray
{
public:
    inline const uint8_t* data() const { return bytes; }
    inline size_t size() const { return N; }
    inline bool operator==(const ByteArray& other) const { return std::memcmp(bytes, other.bytes, sizeof(bytes)) == 0; }
    inline bool operator!=(const ByteArray& other) const { return !(*this == other); }
    inline bool isTheSame(const ByteArray& other) const { return std::memcmp(bytes, other.bytes, sizeof(bytes)) == 0; }
    inline bool isEmpty() const;
protected:
    uint8_t bytes[N];
};

template<std::size_t N>
inline bool ByteArray<N>::isEmpty() const {
  for (auto i = 0; i < N; i++) {
    if (bytes[i] != 0) {
      return false;
    }
  }
  return true;
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
typedef ByteArray<64> Signature;
typedef ByteArray<64> PrivateKey;

#endif // __GRADIDO_BLOCKCHAIN_CRYPTO_PUBLIC_KEY_H