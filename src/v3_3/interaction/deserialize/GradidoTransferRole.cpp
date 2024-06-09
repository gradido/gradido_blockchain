#include "gradido_blockchain/v3_3/interaction/deserialize/GradidoTransferRole.h"
#include "gradido_blockchain/v3_3/interaction/deserialize/TransferAmountRole.h"

namespace gradido {
    namespace v3_3 {
        namespace interaction {
            namespace deserialize {
                GradidoTransferRole::GradidoTransferRole(const GradidoTransferMessage& gradidoTransfer)
                    : mGradidoTransfer(
                        TransferAmountRole(gradidoTransfer["sender"_f].value()).data(),
                        std::make_shared<memory::Block>(gradidoTransfer["recipient"_f].value())
                    )
                {
                    
                }
            }
        }
    }
}