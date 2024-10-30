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
                inline void setCreatedAt(data::Timestamp createdAt) { mCreatedAt = createdAt; }

                //! \param blockchain can be nullptr, so if overloading this function, don't forget to check
                virtual void checkRequiredSignatures(
                    const data::SignatureMap& signatureMap,
                    std::shared_ptr<blockchain::Abstract> blockchain = nullptr
                ) const;
			        
            protected:
                bool isValidCommunityAlias(std::string_view communityAlias) const;
                void validateEd25519PublicKey(memory::ConstBlockPtr ed25519PublicKey, const char* name) const;                    
                void validateEd25519Signature(memory::ConstBlockPtr ed25519Signature, const char* name) const;

				void isPublicKeyForbidden(memory::ConstBlockPtr pubkey) const;

                //! throw if blockchainProvider is null or no blockchain can be found for communityId
                //! \return valid blockchain pointer
                std::shared_ptr<blockchain::Abstract> findBlockchain(
                    blockchain::AbstractProvider* blockchainProvider,
                    std::string_view communityId,
                    const char* callerFunction
                );

                const static std::string mCommunityIdRegexString;
                data::TimestampSeconds mConfirmedAt;
                data::Timestamp mCreatedAt;
				uint32_t mMinSignatureCount;
				std::vector<memory::ConstBlockPtr> mRequiredSignPublicKeys;
				std::vector<memory::ConstBlockPtr> mForbiddenSignPublicKeys;
            };
        }
    }
}



#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_ABSTRACT_ROLE_H