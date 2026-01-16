#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_TRANSFER_AMOUNT_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_TRANSFER_AMOUNT_ROLE_H

#include "AbstractRole.h"
#include "gradido_blockchain/data/CrossGroupType.h"

namespace gradido {
	namespace data {
		class TransferAmount;
	}
	namespace interaction {
		namespace validate {
			class TransferAmountRole : public AbstractRole
			{
			public:
				TransferAmountRole(const data::TransferAmount& transferAmount) : mTransferAmount(transferAmount), mCrossGroupType(data::CrossGroupType::LOCAL) {}

				void run(Type type, ContextData& c);
				inline void setCrossGroupType(data::CrossGroupType crossGroupType) { mCrossGroupType = crossGroupType; }

			protected:
				const data::TransferAmount& mTransferAmount;
				data::CrossGroupType mCrossGroupType;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_TRANSFER_AMOUNT_ROLE_H