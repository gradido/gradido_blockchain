#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_ABSTRACT_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_ABSTRACT_ROLE_H

#include "Type.h"
#include "gradido_blockchain/blockchain/AbstractProvider.h"

namespace gradido {
	namespace interaction {
		namespace validate {

            class AbstractRole 
            {
            public:
                AbstractRole() : mMinSignatureCount(0) {}
                virtual ~AbstractRole() {}
                // test if transaction is valid, throw an exception on error
                virtual void run(
                    Type type = Type::SINGLE,
                    std::string_view communityId = "",
                    blockchain::AbstractProvider* blockchainProvider = nullptr,
                    data::ConstConfirmedTransactionPtr senderPreviousConfirmedTransaction = nullptr,
                    data::ConstConfirmedTransactionPtr recipientPreviousConfirmedTransaction = nullptr
                ) = 0;

                inline void setConfirmedAt(data::TimestampSeconds confirmedAt) { mConfirmedAt = confirmedAt; }

                void checkRequiredSignatures(const data::SignatureMap& signatureMap) const;
			        
            protected:
                bool isValidCommunityAlias(std::string_view communityAlias) const;
                void validateEd25519PublicKey(memory::ConstBlockPtr ed25519PublicKey, const char* name) const;                    
                void validateEd25519Signature(memory::ConstBlockPtr ed25519Signature, const char* name) const;

				void isPublicKeyForbidden(memory::ConstBlockPtr pubkey) const;

                data::TimestampSeconds mConfirmedAt;
				uint32_t mMinSignatureCount;
				std::vector<memory::ConstBlockPtr> mRequiredSignPublicKeys;
				std::vector<memory::ConstBlockPtr> mForbiddenSignPublicKeys;
            };
        }
    }
}



#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_ABSTRACT_ROLE_H