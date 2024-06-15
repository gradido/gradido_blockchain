#include "gradido_blockchain/v3_3/interaction/validate/ConfirmedTransactionRole.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace validate {
				void ConfirmedTransactionRole::run(
					Type type,
					const std::string& communityId,
					std::shared_ptr<AbstractBlockchainProvider> blockchainProvider,
					data::ConfirmedTransactionPtr previousConfirmedTransaction,
					data::ConfirmedTransactionPtr recipientPreviousConfirmedTransaction
				) {

				}
			}
		}
	}
}