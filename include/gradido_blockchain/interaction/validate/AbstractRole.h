#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_ABSTRACT_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_ABSTRACT_ROLE_H

#include "Type.h"
#include "gradido_blockchain/data/compact/PublicKeyIndex.h"
#include "gradido_blockchain/data/Timestamp.h"
#include "gradido_blockchain/memory/Block.h"
#include "ContextData.h"

#include <array>
#include <memory>

namespace gradido {
  namespace blockchain {
    class AbstractProvider;
  }
  namespace data {
    class SignatureMap;
  }
	namespace interaction {
		namespace validate {
            
      class AbstractRole 
      {
      public:
        AbstractRole() 
          : mMinSignatureCount(0), mRequiredSignPublicKeyIndicesCount(0), mForbiddenSignPublicKeyIndicesCount(0), 
          mRequiredSignPublicKeyIndices{}, mForbiddenSignPublicKeyIndices{} 
        {
        }
        virtual ~AbstractRole() {}
        // test if transaction is valid, throw an exception on error
        virtual void run(Type type, ContextData& c) = 0;

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
        void isPublicKeyForbidden(data::compact::PublicKeyIndex publicKeyIndex) const;

        std::vector<data::compact::PublicKeyIndex> getRequiredSignPublicKeyIndicesVector() const;

        //! throw if blockchainProvider is null or no blockchain can be found for communityId
        //! \return valid blockchain pointer
        std::shared_ptr<blockchain::Abstract> findBlockchain(
          blockchain::AbstractProvider* blockchainProvider,
          uint32_t communityIdIndex,
          const char* callerFunction
        );

        data::TimestampSeconds mConfirmedAt;
        data::Timestamp mCreatedAt;
				uint32_t mMinSignatureCount;
        uint8_t  mRequiredSignPublicKeyIndicesCount;
        uint8_t  mForbiddenSignPublicKeyIndicesCount;
				std::vector<memory::ConstBlockPtr> mRequiredSignPublicKeys;
        std::array<data::compact::PublicKeyIndex, 2> mRequiredSignPublicKeyIndices;
				std::vector<memory::ConstBlockPtr> mForbiddenSignPublicKeys;
        std::array<data::compact::PublicKeyIndex, 1> mForbiddenSignPublicKeyIndices;
      };
    }
  }
}



#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_ABSTRACT_ROLE_H