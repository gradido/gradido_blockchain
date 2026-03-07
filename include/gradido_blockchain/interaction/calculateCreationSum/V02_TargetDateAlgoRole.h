#ifndef  __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_CREATION_SUM_V02_TARGET_DATE_ALGO_ROLE_H
#define  __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_CREATION_SUM_V02_TARGET_DATE_ALGO_ROLE_H


#include "AbstractAlgoRole.h"

namespace gradido {
	namespace interaction {
		namespace calculateCreationSum {
			GRADIDOBLOCKCHAIN_EXPORT GradidoUnit v02_TargetDateAlgo(
				Timepoint date,
				Timepoint targetDate,
				data::compact::PublicKeyIndex publicKey,
				const blockchain::Abstract& blockchain,
				uint64_t transactionNrMax = 0
			);

			class V02_TargetDateAlgoRole : public AbstractAlgoRole
			{
			public:
				using AbstractAlgoRole::AbstractAlgoRole;
				//! get creation limit
				virtual GradidoUnit getLimit() const { return 1000.0; }
				//! calculate creation sum
				virtual GradidoUnit run(const blockchain::Abstract& blockchain) const {
					return v02_TargetDateAlgo(mDate, mTargetDate, mPublicKey, blockchain, mTransactionNrMax);
				}
			protected:

			};
		}
	}
}

#endif //  __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_CREATION_SUM_V02_TARGET_DATE_ALGO_ROLE_H
