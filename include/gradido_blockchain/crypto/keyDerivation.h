#ifndef __GRADIDO_BLOCKCHAIN_CRYPTO_KEY_DERIVATION_H
#define __GRADIDO_BLOCKCHAIN_CRYPTO_KEY_DERIVATION_H

#include "gradido_blockchain/memory/Block.h"

#include <array>
#include <span>

namespace keyDerivation {
	std::array<uint8_t, 4> le32(uint32_t i);
	void add28Mul8(std::span<uint8_t, 32> out, std::span<const uint8_t, 32> x, std::span<const uint8_t, 32> y);
	void add256Bits(std::span<uint8_t, 32> out, std::span<const uint8_t, 32> x, std::span<const uint8_t, 32> y);
	memory::Block pointPlus(std::span<const uint8_t, 32> p1, std::span<const uint8_t, 32> p2);
	memory::Block pointOfTrunc32Mul8(std::span<const uint8_t, 32> sk);
}

#endif // __GRADIDO_BLOCKCHAIN_CRYPTO_KEY_DERIVATION_H