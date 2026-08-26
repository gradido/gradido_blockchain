#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_CREATION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_CREATION_ROLE_H

#include "AbstractRole.h"

namespace gradido {
	namespace data {
		class GradidoCreation;
	}

	namespace interaction {
		namespace validate {

			class GradidoCreationRole : public AbstractRole
			{
			public:
				GradidoCreationRole(std::shared_ptr<const data::GradidoCreation> gradidoCreation);

				void validateTargetDate(Timepoint receivedTimePoint);
				void run(Type type, ContextData& c);

				void checkRequiredSignatures(
					const data::SignatureMap& signatureMap,
					std::shared_ptr<blockchain::Abstract> blockchain = nullptr
				) const;
			protected:
				unsigned getTargetDateReceivedDistanceMonth(Timepoint received);

				std::shared_ptr<const data::GradidoCreation> mGradidoCreation;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_CREATION_ROLE_H