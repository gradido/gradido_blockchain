#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_TRANSFER_AMOUNT_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_TRANSFER_AMOUNT_ROLE_H

#include "AbstractRole.h"
#include "gradido_blockchain_core/types/cross_group.h"

namespace gradido {
	namespace data {
		class TransferAmount;
	}
	namespace interaction {
		namespace validate {
			class TransferAmountRole : public AbstractRole
			{
			public:
				TransferAmountRole(const data::TransferAmount& transferAmount) : mTransferAmount(transferAmount), mCrossGroupType(GRDT_CROSS_GROUP_LOCAL) {}

				void run(Type type, ContextData& c);
				inline void setCrossGroupType(grdt_cross_group crossGroupType) { mCrossGroupType = crossGroupType; }

			protected:
				const data::TransferAmount& mTransferAmount;
				grdt_cross_group mCrossGroupType;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_TRANSFER_AMOUNT_ROLE_H