#include "gradido_blockchain/crypto/KeyPairEd25519Ex.h"
#include "ed25519_bip32_c_interface.h"

KeyPairEd25519Ex::KeyPairEd25519Ex(memory::ConstBlockPtr publicKey, memory::ConstBlockPtr privateKey, memory::ConstBlockPtr chainCode, int derivationIndex)
	: KeyPairEd25519(publicKey, privateKey, chainCode), mDerivationIndex(derivationIndex)
{

}

KeyPairEd25519Ex::~KeyPairEd25519Ex()
{
	
}

memory::Block KeyPairEd25519Ex::sign(const unsigned char* message, size_t messageSize) const
{
	auto secret_key = getPrivateKey();
	if (!secret_key) {
		throw Ed25519SignException("missing private key for sign", getPublicKey(), std::string((const char*)message, messageSize));
	}
	memory::Block signature(64);
	sign_extended(message, messageSize, *secret_key, signature);
	return signature;
}

bool KeyPairEd25519Ex::isChildOf(KeyPairEd25519* parent)
{
	auto parent_chain_code = parent->getChainCode();
	if (!parent_chain_code) {
		throw Ed25519DeriveException("missing chain code for child validation", getPublicKey());
	}
		
	auto freshDerived = parent->deriveChild(mDerivationIndex);
	return isTheSame(*freshDerived);
}
