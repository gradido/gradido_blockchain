#ifndef __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_CONTEXT_H
#define __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_CONTEXT_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/v3_3/data/Protocol.h"
#include "TransactionBodyRole.h"
#include "ConfirmedTransactionRole.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace validate {
                class GRADIDOBLOCKCHAIN_EXPORT Context 
                {
                public:
                  Context(const data::TransactionBody& body)
                      : mRole(std::make_unique<TransactionBodyRole>(body)) {}

                  Context(const data::ConfirmedTransaction& body)
                      : mRole(std::make_unique<ConfirmedTransactionRole>(body)) {}

                  //! \param blockchainProvider provide Blockchain access to search for specific transactions
                  void run(
                      Type type = Type::SINGLE,
                      std::string_view communityId = "",
                      std::shared_ptr<blockchain::AbstractProvider> blockchainProvider = nullptr
                  );

                  //! set sender previous confirmed transaction manually, normally last transaction on blockchain will be used
                  inline void setSenderPreviousConfirmedTransaction(data::ConfirmedTransactionPtr senderPreviousConfirmedTransaction) {
                      mSenderPreviousConfirmedTransaction = senderPreviousConfirmedTransaction;
                  }
				  //! set recipient previous confirmed transaction manually, normally last transaction on blockchain will be used, only for cross group transactions
				  inline void setRecipientPreviousConfirmedTransaction(data::ConfirmedTransactionPtr recipientPreviousConfirmedTransaction) {
                      mRecipientPreviousConfirmedTransaction = recipientPreviousConfirmedTransaction;
				  }
                protected:
                    std::unique_ptr<AbstractRole> mRole;
                    std::string mCommunityId;
                    std::shared_ptr<blockchain::AbstractProvider> mBlockchainProvider;
                    data::ConstConfirmedTransactionPtr mSenderPreviousConfirmedTransaction;
                    data::ConstConfirmedTransactionPtr mRecipientPreviousConfirmedTransaction;
                };
      }
    }
  }
}



#endif //__GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_CONTEXT_H