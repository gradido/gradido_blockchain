#include "gradido_blockchain/interaction/deserialize/GradidoTransferRole.h"
#include "gradido_blockchain/interaction/deserialize/TransferAmountRole.h"
#include "gradido_blockchain/interaction/deserialize/Exceptions.h"

namespace gradido {
    namespace interaction {
        namespace deserialize {
			GradidoTransferRole::GradidoTransferRole(const GradidoTransferMessage& gradidoTransfer)
				: mGradidoTransferMessage(gradidoTransfer)
			{

			}

			std::unique_ptr<data::GradidoTransfer> GradidoTransferRole::run() const
            {
				const char* exception = "missing member on deserialize GradidoTransfer";
				if (!mGradidoTransferMessage["sender"_f].has_value()) {
					throw MissingMemberException(exception, "sender");
				}
				if (!mGradidoTransferMessage["recipient"_f].has_value()) {
					throw MissingMemberException(exception, "recipient");
				}
				return std::make_unique<data::GradidoTransfer>(
					TransferAmountRole(mGradidoTransferMessage["sender"_f].value()).data(),
					std::make_shared<memory::Block>(mGradidoTransferMessage["recipient"_f].value())
				);
            }
        }
    }
}