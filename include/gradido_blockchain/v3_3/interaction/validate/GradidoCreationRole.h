#ifndef __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_GRADIDO_CREATION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_GRADIDO_CREATION_ROLE_H

#include "AbstractRole.h"
#include "gradido_blockchain/v3_3/data/Protocol.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace validate {
				class GradidoCreationRole : public AbstractRole
				{
				public:
					GradidoCreationRole(const data::GradidoCreation& gradidoCreation)
						: mGradidoCreation(gradidoCreation) {}

					void validateTargetDate(Timepoint receivedTimePoint);
					void run(Type type = Type::SINGLE, const std::string& communityId, std::shared_ptr<AbstractBlockchainProvider> blockchainProvider);
				protected:
					unsigned getTargetDateReceivedDistanceMonth(Timepoint received);

					const data::GradidoCreation& mGradidoCreation;
				};
			}
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_GRADIDO_CREATION_ROLE_H