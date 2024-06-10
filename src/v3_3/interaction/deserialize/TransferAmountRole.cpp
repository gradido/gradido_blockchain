#include "gradido_blockchain/v3_3/interaction/deserialize/TransferAmountRole.h"
#include "gradido_blockchain/v3_3/interaction/deserialize/Exceptions.h"

namespace gradido {
    namespace v3_3 {
        namespace interaction {
            namespace deserialize {
                TransferAmountRole::TransferAmountRole(const TransferAmountMessage& transferAmount)
                {
                    if (!transferAmount["pubkey"_f].has_value()) {
                        throw MissingMemberException("missing member on deserialize transfer amount", "pubkey");
                    }
                    mTransferAmount = std::make_unique<data::TransferAmount>(
                        std::make_shared<memory::Block>(transferAmount["pubkey"_f].value()),
                        transferAmount["amount"_f].value_or("0"),
                        transferAmount["community_id"_f].value_or("")
                    );
                }
            }
        }
    }
}