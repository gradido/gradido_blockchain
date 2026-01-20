#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_TRANSFER_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_TRANSFER_ROLE_H

#include "AbstractRole.h"
#include "gradido_blockchain/data/CrossGroupType.h"

namespace gradido {
	namespace interaction {
		namespace validate {
			class GradidoTransferRole : public AbstractRole
			{
			public:
				GradidoTransferRole(std::shared_ptr<const data::GradidoTransfer> gradidoTransfer);

				void run(Type type, ContextData& c);
				inline void setCrossGroupType(data::CrossGroupType crossGroupType) { mCrossGroupType = crossGroupType; }

			protected:
				void validatePrevious(
					const data::ConfirmedTransaction& previousConfirmedTransaction,
					std::shared_ptr<blockchain::Abstract> blockchain
				);
				//! both blockchain pointer could be the same
				//! \param senderBlockchain blockchain of sender account
				//! \param recipientBlockchain blockchain of recipient account
				void validateAccount(ContextData& c);

				std::shared_ptr<const data::GradidoTransfer> mGradidoTransfer;
				data::CrossGroupType mCrossGroupType;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_TRANSFER_ROLE_H