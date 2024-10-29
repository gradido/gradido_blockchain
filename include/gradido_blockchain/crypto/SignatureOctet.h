#ifndef __GRADIDO_BLOCKCHAIN_CRYPTO_SIGNATURE_OCTET_H
#define __GRADIDO_BLOCKCHAIN_CRYPTO_SIGNATURE_OCTET_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/memory/Block.h"

/*!
* @author einhornimmond
* @date 22.06.2024
* @brief octet from signature as key for map or unordered_map
* chatGPT: collision probability with first 8 Byte from ED25519 Signatures for signature count = 2^64: ~ 39 %
* it is possible so we need a check for that, but it will happen only rarely, if at all
*/

struct GRADIDOBLOCKCHAIN_EXPORT SignatureOctet
{
	SignatureOctet(memory::ConstBlockPtr signature)
	: octet(0) {
		if (signature && signature->size() >= sizeof(int64_t)) {
			int32_t firstPart = 0;
			int32_t lastPart = 0;

			// Kopiere die ersten 32 Bit (4 Bytes)
			memcpy(&firstPart, signature->data(), sizeof(int32_t));

			// Kopiere die letzten 32 Bit (4 Bytes)
			memcpy(&lastPart, signature->data() + signature->size() - sizeof(int32_t), sizeof(int32_t));

			// Kombiniere die beiden 32-Bit-Werte zu einem 64-Bit-Wert
			octet = (static_cast<int64_t>(firstPart) << 32) | (static_cast<int64_t>(lastPart) & 0xFFFFFFFF);
		}
	}

	// Copy constructor
	SignatureOctet(const SignatureOctet& other)
		: octet(other.octet) {}


	// Copy assignment operator
	SignatureOctet& operator=(const SignatureOctet& other) {
		octet = other.octet;
		return *this;
	}

	inline bool operator<(const SignatureOctet& ob) const {
		return octet < ob.octet;
	}

	inline bool operator==(const SignatureOctet& ob) const {
		return octet == ob.octet;
	}
	int64_t octet;
};


// Hash function for SignatureOctet
namespace std {
	template <>
	struct hash<SignatureOctet>
	{
		std::size_t operator()(const SignatureOctet& s) const noexcept {
			return std::hash<int64_t>()(s.octet);
		}
	};
}

#endif //#define __GRADIDO_BLOCKCHAIN_CRYPTO_SIGNATURE_OCTET_H