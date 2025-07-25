#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_CREATION_SUM_CONTEXT_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_CREATION_SUM_CONTEXT_H

#include "gradido_blockchain/export.h"
#include "AbstractAlgoRole.h"

namespace gradido {
	namespace interaction {		
		namespace calculateCreationSum {

			class GRADIDOBLOCKCHAIN_EXPORT Context
			{
			public:
				Context(Timepoint date, Timepoint targetDate, memory::ConstBlockPtr publicKey, uint64_t transactionNrMax = 0);
				// move constructor & assignment
				Context(Context&&) noexcept = default;
				Context& operator=(Context&&) noexcept = default;

				// delete copy constructor & assignment
				Context(const Context&) = delete;
				Context& operator=(const Context&) = delete;

				GradidoUnit run(blockchain::Abstract& blockchain) { return mRole->run(blockchain); }
				GradidoUnit getLimit() { return mRole->getLimit(); }

			protected:
				std::unique_ptr<AbstractAlgoRole> mRole;
			};
		}
	}
}


#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_CREATION_SUM_CONTEXT_H