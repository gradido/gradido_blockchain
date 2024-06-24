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
			memcpy(&octet, *signature, sizeof(int64_t));
		}
	}
	inline bool operator<(const SignatureOctet& ob) const {
		return octet < ob.octet;
	}
	int64_t octet;
};

#endif //#define __GRADIDO_BLOCKCHAIN_CRYPTO_SIGNATURE_OCTET_H