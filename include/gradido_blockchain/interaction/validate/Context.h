#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_CONTEXT_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_CONTEXT_H

#include "gradido_blockchain/export.h"
#include "AbstractRole.h"
#include "Error.h"
#include "Options.h"
#include "Type.h"
#include <memory>

namespace gradido {
    class AppContext;
    namespace data {
      class TransactionBody;
      class GradidoTransaction;
      class ConfirmedTransaction;
      namespace compact {
        class ConfirmedGradidoTx;
      }
    }
    namespace blockchain {
      class Abstract;
    }
	namespace interaction {
		namespace validate {


      // rewrite as function with compact confirmed tx
      GRADIDOBLOCKCHAIN_EXPORT Error validate(
        const data::compact::ConfirmedGradidoTx& tx, 
        const AppContext& appContext,
        Options options
      );

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

          // move constructor & assignment
          Context(Context&&) noexcept = default;
          Context& operator=(Context&&) noexcept = default;

          // delete copy constructor & assignment
          Context(const Context&) = delete;
          Context& operator=(const Context&) = delete;

          inline void disableVerify() { mDisableVerify = true; }
          inline void disableRunningHashTest() { mDisableRunningHashTest = true; }

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
          bool mDisableVerify;
          bool mDisableRunningHashTest;
      };
    }
  }
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_CONTEXT_H