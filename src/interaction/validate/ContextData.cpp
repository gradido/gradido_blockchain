#include "gradido_blockchain/interaction/validate/ContextData.h"

namespace gradido {
  namespace interaction {
    namespace validate {

      ContextData::ContextData()
      : senderBlockchain(nullptr), 
        recipientBlockchain(nullptr), 
        senderPreviousConfirmedTransaction(nullptr), 
        recipientPreviousConfirmedTransaction(nullptr)
      {
      }

      ContextData::ContextData(
        std::shared_ptr<blockchain::Abstract> _senderBlockchain,
        std::shared_ptr<const data::ConfirmedTransaction> _senderPreviousConfirmedTransaction
      ) : senderBlockchain(_senderBlockchain),
        recipientBlockchain(_senderBlockchain),
        senderPreviousConfirmedTransaction(_senderPreviousConfirmedTransaction),
        recipientPreviousConfirmedTransaction(_senderPreviousConfirmedTransaction)
      {

      }

      ContextData::~ContextData()
      {
      }

    }
  }
}