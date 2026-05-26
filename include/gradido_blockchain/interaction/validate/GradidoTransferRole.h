#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_TRANSFER_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_TRANSFER_ROLE_H

#include "AbstractRole.h"
#include "gradido_blockchain_core/types/cross_group.h"

namespace gradido {
	namespace data {
		class GradidoTransfer;
	}
	namespace interaction {
		namespace validate {
			class GradidoTransferRole : public AbstractRole
			{
			public:
				GradidoTransferRole(std::shared_ptr<const data::GradidoTransfer> gradidoTransfer);

				void run(Type type, ContextData& c);
				inline void setCrossGroupType(grdt_cross_group crossGroupType) { mCrossGroupType = crossGroupType; }

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
				grdt_cross_group mCrossGroupType;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_TRANSFER_ROLE_H