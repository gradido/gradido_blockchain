#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_CREATION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_CREATION_ROLE_H

#include "AbstractRole.h"

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
				GradidoCreationRole(std::shared_ptr<const data::GradidoCreation> gradidoCreation);

				void validateTargetDate(Timepoint receivedTimePoint);
				void run(
					Type type,
					std::string_view communityId,
					blockchain::AbstractProvider* blockchainProvider,
					data::ConstConfirmedTransactionPtr senderPreviousConfirmedTransaction,
					data::ConstConfirmedTransactionPtr recipientPreviousConfirmedTransaction
				);

				void checkRequiredSignatures(
					const data::SignatureMap& signatureMap,
					std::shared_ptr<blockchain::Abstract> blockchain = nullptr
				) const;
			protected:
				GradidoUnit calculateCreationSum(
					memory::ConstBlockPtr accountPubkey,
					date::month month,
					date::year year,
					Timepoint received,
					std::shared_ptr<blockchain::Abstract> blockchain,
					uint64_t maxTransactionNr
				);
				GradidoUnit calculateCreationSumLegacy(
					memory::ConstBlockPtr accountPubkey,
					Timepoint received,
					std::shared_ptr<blockchain::Abstract> blockchain,
					uint64_t maxTransactionNr
				);
				unsigned getTargetDateReceivedDistanceMonth(Timepoint received);
				CreationMaxAlgoVersion getCorrectCreationMaxAlgo(const data::TimestampSeconds& timepoint);

				std::shared_ptr<const data::GradidoCreation> mGradidoCreation;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_CREATION_ROLE_H