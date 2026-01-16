#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_CONTEXT_DATA_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_CONTEXT_DATA_H

#include <memory>

namespace gradido {
    namespace data {
      class ConfirmedTransaction;
    }
    namespace blockchain {
      class Abstract;
    }
	namespace interaction {
		namespace validate {
      struct ContextData
      {
        ContextData();
        ContextData(
          std::shared_ptr<blockchain::Abstract> senderBlockchain, 
          std::shared_ptr<const data::ConfirmedTransaction> senderPreviousConfirmedTransaction
        );
        ~ContextData();
        std::shared_ptr<blockchain::Abstract> senderBlockchain;
        std::shared_ptr<blockchain::Abstract> recipientBlockchain;
        std::shared_ptr<const data::ConfirmedTransaction> senderPreviousConfirmedTransaction;
        std::shared_ptr<const data::ConfirmedTransaction> recipientPreviousConfirmedTransaction;
      };
    }
  }
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_CONTEXT_DATA_H