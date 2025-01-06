#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_CREATION_SUM_V01_THREE_MONTHS_ALGO_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_CREATION_SUM_V01_THREE_MONTHS_ALGO_ROLE_H

#include "AbstractAlgoRole.h"

namespace gradido {
	namespace interaction {
		namespace calculateCreationSum {
			class V01_ThreeMonthsAlgoRole : public AbstractAlgoRole
			{
			public:
				using AbstractAlgoRole::AbstractAlgoRole;
				//! get creation limit
				virtual GradidoUnit getLimit() const { return GradidoUnit::fromDecimal(3000.0);  }
				//! calculate creation sum
				virtual GradidoUnit run(const blockchain::Abstract& blockchain) const;
			};
		}
	}
}

#endif // __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_CREATION_SUM_V01_THREE_MONTHS_ALGO_ROLE_H
