#include "gradido_blockchain/crypto/keyDerivation.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"

namespace keyDerivation {
    std::array<uint8_t, 4> le32(uint32_t i) 
    {
        return {
            static_cast<uint8_t>(i),
            static_cast<uint8_t>(i >> 8),
            static_cast<uint8_t>(i >> 16),
            static_cast<uint8_t>(i >> 24)
        };
    }

    void add28Mul8(std::span<uint8_t, 32> out, std::span<const uint8_t, 32> x, std::span<const uint8_t, 32> y)
    {
        uint16_t carry = 0;

        for (size_t i = 0; i < 28; ++i) {
            uint16_t r = static_cast<uint16_t>(x[i]) + (static_cast<uint16_t>(y[i]) << 3) + carry;
            out[i] = static_cast<uint8_t>(r & 0xff);
            carry = r >> 8;
        }

        for (size_t i = 28; i < 32; ++i) {
            uint16_t r = static_cast<uint16_t>(x[i]) + carry;
            out[i] = static_cast<uint8_t>(r & 0xff);
            carry = r >> 8;
        }
    }

    void add256Bits(std::span<uint8_t, 32> out, std::span<const uint8_t, 32> x, std::span<const uint8_t, 32> y)
    {
        uint16_t carry = 0;

        for (size_t i = 0; i < 32; ++i) {
            uint16_t r = static_cast<uint16_t>(x[i]) + (static_cast<uint16_t>(y[i])) + carry;
            out[i] = static_cast<uint8_t>(r);
            carry = r >> 8;
        }
    }
    memory::Block pointPlus(std::span<const uint8_t, 32> p1, std::span<const uint8_t, 32> p2)
    {
        memory::Block out(32);
        if (crypto_core_ed25519_add(out, p1.data(), p2.data())) {
            throw Ed25519DeriveException("Invalid Addition", nullptr);
        }
        out[32] ^= 0x80;
        return out;
    }

    memory::Block pointOfTrunc32Mul8(std::span<const uint8_t, 32> sk)
    {
        memory::Block out(32);
        memory::Block copy(32);
        auto copySpan = std::span<uint8_t, 32>{ copy.data(0), 32 };
        // we use out here because it is currently empty, all values 0 
        auto outSpan = std::span<const uint8_t, 32>{ out.data(0), 32 };
        add28Mul8(copySpan, outSpan, sk);
        crypto_scalarmult_base(out, copy);
        return out;
    }
}