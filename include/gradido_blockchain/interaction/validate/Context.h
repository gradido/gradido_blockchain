#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_CONTEXT_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_CONTEXT_H

#include "gradido_blockchain/export.h"
#include "Type.h"
#include <memory>

namespace gradido {
    namespace data {
        class TransactionBody;
        class GradidoTransaction;
        class ConfirmedTransaction;
    }
    namespace blockchain {
        class Abstract;
    }
	namespace interaction {
		namespace validate {
            class AbstractRole;
            class TransactionBodyRole;
            class GradidoTransactionRole;
            class ConfirmedTransactionRole;

            class GRADIDOBLOCKCHAIN_EXPORT Context 
            {
            public:
                Context(const data::TransactionBody& body);
                Context(const data::GradidoTransaction& body);
                Context(const data::ConfirmedTransaction& body);
                ~Context();

                //! \param blockchainProvider provide Blockchain access to search for specific transactions
                void run(Type type = Type::SINGLE, std::shared_ptr<blockchain::Abstract> blockchain = nullptr);

                //! set sender previous confirmed transaction manually, normally last transaction on blockchain will be used
                inline void setSenderPreviousConfirmedTransaction(std::shared_ptr<const data::ConfirmedTransaction> senderPreviousConfirmedTransaction) {
                    mSenderPreviousConfirmedTransaction = senderPreviousConfirmedTransaction;
                }
				//! set recipient previous confirmed transaction manually, normally last transaction on blockchain will be used, only for cross group transactions
				inline void setRecipientPreviousConfirmedTransaction(std::shared_ptr<const data::ConfirmedTransaction> recipientPreviousConfirmedTransaction) {
                    mRecipientPreviousConfirmedTransaction = recipientPreviousConfirmedTransaction;
				}
            protected:
                std::unique_ptr<AbstractRole> mRole;
                std::shared_ptr<const data::ConfirmedTransaction> mSenderPreviousConfirmedTransaction;
                std::shared_ptr<const data::ConfirmedTransaction> mRecipientPreviousConfirmedTransaction;
            };
        }
    }
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_CONTEXT_H