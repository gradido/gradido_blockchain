#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/const.h"
#include "gradido_blockchain/data/TransferAmount.h"
#include "gradido_blockchain/interaction/validate/TransferAmountRole.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"

#include <memory>
using std::shared_ptr;

namespace gradido {
	namespace interaction {
		namespace validate {
			void TransferAmountRole::run(Type type, ContextData& c) {
				if ((type & Type::SINGLE) == Type::SINGLE) {
					if (mTransferAmount.getAmount() <= GradidoUnit::zero()) {
						throw TransactionValidationInvalidInputException("zero or negative amount", "amount", "string");
					}
					validateEd25519PublicKey(mTransferAmount.getPublicKey(), "sender");
				}
			}
		}
	}
}
