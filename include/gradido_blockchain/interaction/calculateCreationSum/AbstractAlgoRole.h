#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_CREATION_SUM_ABSTRACT_ALGO_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_CREATION_SUM_ABSTRACT_ALGO_ROLE_H

#include "gradido_blockchain/GradidoUnit.h"

namespace gradido {
	namespace blockchain {
		class Abstract;
	}
	namespace interaction {
		namespace calculateCreationSum {
			class AbstractAlgoRole
			{
			public:
				//! \param date createdAt for checking sum before making transaction, and confirmedAt for validate confirmed transaction
				AbstractAlgoRole(Timepoint date, Timepoint targetDate, memory::ConstBlockPtr publicKey, uint64_t transactionNrMax = 0)
					: mDate(date), mTargetDate(targetDate), mPublicKey(publicKey), mTransactionNrMax(transactionNrMax) {}

				//! get creation limit 
				virtual GradidoUnit getLimit() const = 0;
				//! calculate creation sum 
				virtual GradidoUnit run(const blockchain::Abstract& blockchain) const = 0;

			protected:
				Timepoint mDate;
				Timepoint mTargetDate;
				memory::ConstBlockPtr mPublicKey;
				uint64_t  mTransactionNrMax;
			};				
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_CALCULATE_CREATION_SUM_ABSTRACT_ALGO_ROLE_H