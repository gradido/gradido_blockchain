#ifndef __GRADIDO_LOGIN_SERVER_CRYPTO_ED25519_EXTENDED_H
#define __GRADIDO_LOGIN_SERVER_CRYPTO_ED25519_EXTENDED_H

#include "KeyPairEd25519.h"

/*!
 * \author: Dario Rekowski
 *
 * \date: 2021-07-01
 *
 * \brief: Key Pairs class for ed25519 extended keys, mainly used for derivation, bip32
*/

class KeyPairEd25519Ex: public KeyPairEd25519
{
public:
	//! \param privateKey: take ownership, release after object destruction
	//! \param publicKey: copy
	KeyPairEd25519Ex(memory::ConstBlockPtr publicKey, memory::ConstBlockPtr privateKey, memory::ConstBlockPtr chainCode, uint32_t derivationIndex);

	 ~KeyPairEd25519Ex();

	bool isChildOf(KeyPairEd25519* parent);

	inline uint32_t getDerivationIndex() const { return mDerivationIndex; }
	inline Ed25519DerivationType getDerivationType() const { return KeyPairEd25519::getDerivationType(mDerivationIndex); }
	//! derivation with public key and chaincode only possible
	inline bool isSoftDerivated() const { return getDerivationType() == Ed25519DerivationType::SOFT; }
	//! derivation only with extended secret key possible
	inline bool isHardDerivated() const { return getDerivationType() == Ed25519DerivationType::HARD; }

protected:

private:
	uint32_t mDerivationIndex;
};

#endif //__GRADIDO_LOGIN_SERVER_CRYPTO_ED25519_EXTENDED_H