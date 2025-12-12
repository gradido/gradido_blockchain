#include "gradido_blockchain/crypto/SignatureOctet.h"
#include "gradido_blockchain/memory/Block.h"

using memory::Block;

SignatureOctet::SignatureOctet()
  : octet(0)
{

}

SignatureOctet::SignatureOctet(const Block& signature)
{
    if (!signature.hash().empty()) {
        octet = signature.hash().octet;
    }
    else {
        octet = calculateOctet(signature.data(), signature.size());
    }
}

SignatureOctet::SignatureOctet(const std::string& binString)
  : SignatureOctet((const uint8_t*)binString.data(), binString.size())
{

}

SignatureOctet::~SignatureOctet()
{

}

SignatureOctet::SignatureOctet(const uint8_t* data, size_t size)
  : octet(calculateOctet(data, size)) {
}

// Copy constructor
SignatureOctet::SignatureOctet(const SignatureOctet& other)
: octet(other.octet)
{

}

// Copy assignment operator
SignatureOctet& SignatureOctet::operator=(const SignatureOctet& other)
{
  octet = other.octet;
  return *this;
}

int64_t SignatureOctet::calculateOctet(const uint8_t* data, size_t size) const
{
    if (data && size >= sizeof(int64_t)) {
        int32_t firstPart = 0;
        int32_t lastPart = 0;

        // Copy the first 32 bits (4 bytes)
        memcpy(&firstPart, data, sizeof(int32_t));

        // Copy the last 32 bits (4 bytes)
        memcpy(&lastPart, data + size - sizeof(int32_t), sizeof(int32_t));

        // Combine the two 32-bit values into a 64-bit value
        return (static_cast<int64_t>(firstPart) << 32) | (static_cast<int64_t>(lastPart) & 0xFFFFFFFF);
    }
    return 0;
}