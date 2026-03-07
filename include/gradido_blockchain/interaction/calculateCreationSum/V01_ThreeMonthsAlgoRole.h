#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_CREATION_SUM_V01_THREE_MONTHS_ALGO_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_CREATION_SUM_V01_THREE_MONTHS_ALGO_ROLE_H

#include "AbstractAlgoRole.h"

namespace date {
	class year_month;
}

namespace gradido {
	namespace blockchain {
		class Abstract;
	}
	namespace interaction {
		namespace calculateCreationSum {
			GRADIDOBLOCKCHAIN_EXPORT GradidoUnit v01_ThreeMonthsAlgo(
				date::year_month date,
				data::compact::PublicKeyIndex publicKey,
				const blockchain::Abstract& blockchain,
				uint64_t transactionNrMax = 0
			);

			class V01_ThreeMonthsAlgoRole : public AbstractAlgoRole
			{
			public:
				using AbstractAlgoRole::AbstractAlgoRole;
				//! get creation limit
				virtual GradidoUnit getLimit() const { return 3000.0;  }
				//! calculate creation sum
				virtual GradidoUnit run(const blockchain::Abstract& blockchain) const;
			};
		}
	}
}

#endif // __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_CREATION_SUM_V01_THREE_MONTHS_ALGO_ROLE_H
