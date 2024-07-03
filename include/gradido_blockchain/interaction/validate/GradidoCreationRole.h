#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_CREATION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_CREATION_ROLE_H

#include "AbstractRole.h"
#include "gradido_blockchain/data/Protocol.h"

namespace gradido {
	namespace interaction {
		namespace validate {

			enum class CreationMaxAlgoVersion : short
			{
				v01_THREE_MONTHS_3000_GDD,
				v02_ONE_MONTH_1000_GDD_TARGET_DATE
			};

			class GradidoCreationRole : public AbstractRole
			{
			public:
				GradidoCreationRole(const data::GradidoCreation& gradidoCreation);

				void validateTargetDate(Timepoint receivedTimePoint);
				void run(
					Type type,
					std::string_view communityId,
					blockchain::AbstractProvider* blockchainProvider,
					data::ConstConfirmedTransactionPtr senderPreviousConfirmedTransaction,
					data::ConstConfirmedTransactionPtr recipientPreviousConfirmedTransaction
				);
			protected:
				Decimal calculateCreationSum(
					memory::ConstBlockPtr accountPubkey,
					date::month month,
					date::year year,
					Timepoint received,
					std::shared_ptr<blockchain::Abstract> blockchain,
					uint64_t maxTransactionNr
				);
				Decimal calculateCreationSumLegacy(
					memory::ConstBlockPtr accountPubkey,
					Timepoint received,
					std::shared_ptr<blockchain::Abstract> blockchain,
					uint64_t maxTransactionNr
				);
				unsigned getTargetDateReceivedDistanceMonth(Timepoint received);
				CreationMaxAlgoVersion getCorrectCreationMaxAlgo(const data::TimestampSeconds& timepoint);

				const data::GradidoCreation& mGradidoCreation;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_CREATION_ROLE_H