#include "gradido_blockchain/data/CommunityRoot.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"

namespace gradido {
	namespace data {
		CommunityRoot::CommunityRoot(
			memory::ConstBlockPtr pubkeyPtr,
			memory::ConstBlockPtr gmwPubkeyPtr,
			memory::ConstBlockPtr aufPubkeyPtr
		) : mPubkey(pubkeyPtr), mGmwPubkey(gmwPubkeyPtr), mAufPubkey(aufPubkeyPtr) 
		{
			KeyPairEd25519::validatePublicKey(pubkeyPtr);
			KeyPairEd25519::validatePublicKey(gmwPubkeyPtr);
			KeyPairEd25519::validatePublicKey(aufPubkeyPtr);

			const auto& pubkey = *pubkeyPtr;
			const auto& gmwPubkey = *gmwPubkeyPtr;
			const auto& aufPubkey = *aufPubkeyPtr;

			if (gmwPubkey == aufPubkey) { 
				throw GradidoNodeInvalidDataException("gmw and auf are the same"); 
			}
			if (pubkey == gmwPubkey) {
				throw GradidoNodeInvalidDataException("gmw and pubkey are the same"); 
			}
			if (aufPubkey == pubkey) { 
				throw GradidoNodeInvalidDataException("aufPubkey and pubkey are the same"); 
			}
		}

		bool CommunityRoot::isInvolved(memory::ConstBlockPtr publicKey) const
		{
			return
				publicKey->isTheSame(mPubkey) ||
				publicKey->isTheSame(mGmwPubkey) ||
				publicKey->isTheSame(mAufPubkey);
		}
	}
}