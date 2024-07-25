#include "gradido_blockchain/interaction/deserialize/GradidoTransferRole.h"
#include "gradido_blockchain/interaction/deserialize/TransferAmountRole.h"
#include "gradido_blockchain/interaction/deserialize/Exceptions.h"

namespace gradido {
    namespace interaction {
        namespace deserialize {
            GradidoTransferRole::GradidoTransferRole(const GradidoTransferMessage& gradidoTransfer)
            {
				const char* exception = "missing member on deserialize GradidoTransfer";
				if (!gradidoTransfer["sender"_f].has_value()) {
					throw MissingMemberException(exception, "sender");
				}
				if (!gradidoTransfer["recipient"_f].has_value()) {
					throw MissingMemberException(exception, "recipient");
				}
				mGradidoTransfer = std::make_unique<data::GradidoTransfer>(
					TransferAmountRole(gradidoTransfer["sender"_f].value()).data(),
					std::make_shared<memory::Block>(gradidoTransfer["recipient"_f].value())
				);
            }
        }
    }
}