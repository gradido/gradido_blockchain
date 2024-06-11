#ifndef __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_CONTEXT_H
#define __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_CONTEXT_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/v3_3/data/Protocol.h"
#include "gradido_blockchain/v3_3/AbstractBlockchainProvider.h"
#include "TransactionBodyRole.h"
#include "Type.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace validate {
                class GRADIDOBLOCKCHAIN_EXPORT Context 
                {
                public:
                    //! \param blockchainProvider provide Blockchain access to search for specific transactions
                  Context(const data::TransactionBody& body, const std::string& communityId = "", std::shared_ptr<AbstractBlockchainProvider> blockchainProvider = nullptr)
                      : mBlockchainProvider(blockchainProvider) {}

                  void run(const data::TransactionBody& body, Type type = Type::SINGLE, const std::string& communityId = "");
                  void run(const data::ConfirmedTransaction& confirmedTransaction, Type type = Type::SINGLE, const std::string& communityId = "");
                  void run(const data::GradidoTransaction& gradidoTransaction, Type type = Type::SINGLE, const std::string& communityId = "");
                protected:
                    std::shared_ptr<AbstractBlockchainProvider> mBlockchainProvider;
                };
      }
    }
  }
}



#endif //__GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_CONTEXT_H